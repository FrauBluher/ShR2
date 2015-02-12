/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Pavlo Milo Manovi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/**
 * @file	DMA_Transfer.c
 * @author 	Pavlo Milo Manovi
 * @date	April, 2014
 * @brief 	This library provides implementation of methods to transmit data over
 *		SPI and UART with DMA.  Note, in a system using SPI and DMA block
 *		transfers the SPI must be the master on the bus.
 *
 *		Note: This means that I need to write an SPI driver on the BBB using
 *		the PRU as the linux kernel isn't well suited to interrupts and a slave
 *		configuration.... Future work!
 */

#include <xc.h>
#include "DMA_Transfer.h"
#include "ADCModuleBoard.h"
#include <plib.h>

#define SYS_FREQ (80000000L)
#define	GetPeripheralClock()		(SYS_FREQ/(1 << OSCCONbits.PBDIV))

uint8_t inited = 0;
uint8_t currentBuffer = BUFFER_A;
uint32_t crc = 0;
SampleBuffer *BufA;
SampleBuffer *BufB;

uint8_t testBuffer[] = "hello world!\r\n";

DmaChannel uartRxChn = DMA_CHANNEL0;
DmaChannel spiTxChn = DMA_CHANNEL1;
DmaChannel spiRxChn = DMA_CHANNEL2;
DmaChannel uartTxChn = DMA_CHANNEL3;
DmaChannel crcChn = DMA_CHANNEL4;
DmaChannel pmpChn = DMA_CHANNEL5;

uint8_t dmaBuff[256] = {};

uint8_t BufferToUART_Init(void)
{
	UARTConfigure(UART3, UART_ENABLE_PINS_TX_RX_ONLY | UART_ENABLE_HIGH_SPEED);
	UARTSetLineControl(UART3, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
	UARTSetDataRate(UART3, GetPeripheralClock(), 921600);
	UARTEnable(UART3, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));

	DmaChnOpen(uartTxChn, DMA_CHN_PRI2, DMA_OPEN_DEFAULT);

	//DMA continues transfer after TX FIFO is emptied.
	DmaChnSetEventControl(uartTxChn, DMA_EV_START_IRQ_EN | DMA_EV_START_IRQ(_UART3_TX_IRQ));

	INTEnable(INT_SOURCE_DMA(uartTxChn), INT_ENABLED); // enable the chn interrupt in the INT controller

	inited = 1;
	return(EXIT_SUCCESS);
}

uint8_t BufferToSpi_Init(SampleBuffer *BufferA, SampleBuffer *BufferB)
{
	if (!inited) {
		BufA = BufferA;
		BufB = BufferB;

		BufA->index = 0;
		BufB->index = 0;

		// open and configure the SPI channel to use: master, no frame mode, 8 bit mode.
		// we'll be using 80MHz/4 = 20MHz SPI peripheral base clock
		SpiChnOpen(SPI_CHANNEL1, SPI_OPEN_MSTEN | SPI_OPEN_SMP_END |
			SPI_OPEN_CKE_REV | SPI_OPEN_MODE8, 4);

		DmaChnOpen(spiTxChn, DMA_CHN_PRI2, DMA_OPEN_DEFAULT);
		DmaChnOpen(spiRxChn, DMA_CHN_PRI3, DMA_OPEN_DEFAULT);

		//DMA continues transfer after TX FIFO is emptied.
		DmaChnSetEventControl(spiTxChn, DMA_EV_START_IRQ(_SPI1_TX_IRQ));
		DmaChnSetEventControl(spiRxChn, DMA_EV_START_IRQ(_SPI1_RX_IRQ));

		DmaChnSetEvEnableFlags(spiTxChn, DMA_EV_BLOCK_DONE);
		DmaChnSetEvEnableFlags(spiRxChn, DMA_EV_BLOCK_DONE | DMA_EV_DST_FULL);

		DmaChnClrEvFlags(spiTxChn, DMA_EV_ALL_EVNTS);
		DmaChnClrEvFlags(spiRxChn, DMA_EV_ALL_EVNTS);

		DmaChnSetIntPriority(spiRxChn, 5, 3);
		DmaChnIntEnable(spiRxChn);
		DmaChnClrIntFlag(spiRxChn);


		inited = 1;
		return(EXIT_SUCCESS);
	} else {
		return(EXIT_FAILURE);
	}
}

void BufferToPMP_Init(void)
{
	mPMPOpen(PMP_ON | PMP_MUX_OFF | PMP_READ_WRITE_EN | PMP_CS2_EN |
		PMP_CS2_POL_LO | PMP_WRITE_POL_LO | PMP_READ_POL_LO,
		PMP_IRQ_READ_WRITE | PMP_AUTO_ADDR_OFF |
		PMP_DATA_BUS_8 | PMP_MODE_MASTER1 | PMP_WAIT_BEG_3 |
		PMP_WAIT_MID_7 | PMP_WAIT_END_3, PMP_PEN_OFF,
		PMP_INT_OFF);
	PMPSetAddress(0x8000); //Not needed?
	DmaChnOpen(pmpChn, 0, DMA_OPEN_DEFAULT);
	DmaChnSetEventControl(pmpChn, DMA_EV_START_IRQ(_PMP_IRQ));
	DmaChnSetEvEnableFlags(pmpChn, DMA_EV_BLOCK_DONE);
	DmaChnSetIntPriority(pmpChn, 5, 3);
	DmaChnIntEnable(pmpChn);
	DmaChnEnable(pmpChn);
}

void BufferToPMP_TransferA(uint16_t transferSize)
{
	DmaChnSetTxfer(pmpChn, BufA->BufferArray, (void*) &PMDIN, transferSize, 1, 1);
	DmaChnStartTxfer(pmpChn, DMA_WAIT_NOT, 0);
}

void BufferToPMP_TransferB(uint16_t transferSize)
{
	DmaChnSetTxfer(pmpChn, BufB->BufferArray, (void*) &PMDIN, transferSize, 1, 1);
	DmaChnStartTxfer(pmpChn, DMA_WAIT_NOT, 0);
}

void StartSPIAcquisition(uint8_t buffer)
{
	SPI1STAT = 0; //So we don't lose events.

	if (buffer == BUFFER_A) {
		DmaChnSetTxfer(spiRxChn, (void *) &SPI1BUF, BufA->BufferArray, 1, BUFFERLENGTH, 1);
	} else if (buffer == BUFFER_B) {
		DmaChnSetTxfer(spiRxChn, (void *) &SPI1BUF, BufB->BufferArray, 1, BUFFERLENGTH, 1);
	}
	DmaChnEnable(spiRxChn);
}

void DMAStartUARTRX(void)
{
	DmaChnOpen(uartRxChn, DMA_CHN_PRI1, DMA_OPEN_MATCH); //High priority for control.

	DmaChnSetMatchPattern(uartRxChn, '\r');

	DmaChnSetEventControl(uartRxChn, DMA_EV_START_IRQ_EN | DMA_EV_MATCH_EN | DMA_EV_START_IRQ(_UART3_RX_IRQ));

	DmaChnSetTxfer(uartRxChn, (void*) &U3RXREG, dmaBuff, 1, 256, 1);

	DmaChnSetEvEnableFlags(uartRxChn, DMA_EV_BLOCK_DONE);

	DmaChnSetIntPriority(uartRxChn, INT_PRIORITY_LEVEL_5, INT_SUB_PRIORITY_LEVEL_3);
	DmaChnIntEnable(uartRxChn);
	DmaChnClrIntFlag(uartRxChn);
	DmaChnEnable(uartRxChn);
}

uint8_t BufferToSpi_Transfer(uint8_t *txBuffer, uint16_t transferSize)
{
	DmaChnSetTxfer(DMA_CHANNEL1, txBuffer, (void*) &SPI1BUF, transferSize, 1, 1);

	DmaChnStartTxfer(DMA_CHANNEL1, DMA_WAIT_NOT, 0);
}

void DMA_CRC_Calc(uint8_t *data, uint16_t dataSize)
{
	//CRC Calculation has up until the end of the UART transfer to finish
	//and append its CRC checksum.
	//Standard CCITT CRC 16 polynomial: X^16+X^12+X^5+1, hex=0x00001021  11021?
	DmaChnOpen(crcChn, DMA_CHN_PRI2, DMA_OPEN_DEFAULT);
	DmaChnSetTxfer(crcChn, data, &crc, dataSize, dataSize, dataSize);
	mCrcConfigure(0x1021, 16, 0xffff); // initial seed set to 0xffff
	CrcAttachChannel(crcChn, 1);

	DmaChnSetEvEnableFlags(crcChn, DMA_EV_BLOCK_DONE);
	DmaChnSetIntPriority(crcChn, INT_PRIORITY_LEVEL_5, INT_SUB_PRIORITY_LEVEL_3);
	DmaChnIntEnable(crcChn);
	DmaChnClrIntFlag(crcChn);
	DmaChnEnable(crcChn);

	DmaChnStartTxfer(crcChn, DMA_WAIT_NOT, 0);
}

uint8_t BufferToUART_TransferA(uint16_t transferSize)
{
	// set the transfer source and dest addresses, source and dest size and cell size
	DmaChnSetTxfer(uartTxChn, BufA->BufferArray, (void*) &U3TXREG, transferSize, 1, 1);
	DmaChnSetEvEnableFlags(uartTxChn, DMA_EV_BLOCK_DONE); // enable the transfer done interrupt: pattern match or all the characters transferred
	DmaChnStartTxfer(uartTxChn, DMA_WAIT_NOT, 0); // force the DMA transfer: the UART1 tx flag it's already been active
}

uint8_t BufferToUART_TransferB(uint16_t transferSize)
{
	DmaChnSetTxfer(uartTxChn, BufB->BufferArray, (void*) &U3TXREG, transferSize, 1, 1);
	DmaChnSetEvEnableFlags(uartTxChn, DMA_EV_BLOCK_DONE); // enable the transfer done interrupt: all the characters transferred
	DmaChnStartTxfer(uartTxChn, DMA_WAIT_NOT, 0);
}

void __ISR(_DMA0_VECTOR) DmaHandler0(void)
{
	//StartSPIAcquisition(BUFFER_A);

	//PUT CONTROLS FOR THE DAQ HERE.  CONFIG, ETC.

	DmaChnClrEvFlags(DMA_CHANNEL0, DMA_EV_BLOCK_DONE);
	INTClearFlag(INT_SOURCE_DMA(DMA_CHANNEL0));
}


// Here we handle the events being generated by our SPI TX DMA channel.

void __ISR(_DMA1_VECTOR) DmaHandler1(void)
{
	INTClearFlag(INT_SOURCE_DMA(DMA_CHANNEL1));
}

// Handle the case where we've filled up our destination address with data from the MCP
// here we should change what buffer the ADC data is being written to, and start a DMA
// transfer over uart (or eventually the USB FIFO when that's done).

void __ISR(_DMA2_VECTOR) DmaHandler2(void)
{
	LATBbits.LATB6 ^= 1;
        //starts the DMA transfer over uart from the current buffer

	if (currentBuffer == BUFFER_A) {
		StartSPIAcquisition(BUFFER_B);
		DMA_CRC_Calc(BufA->BufferArray, BUFFERLENGTH);
	} else if (currentBuffer == BUFFER_B) {
		StartSPIAcquisition(BUFFER_A);
		DMA_CRC_Calc(BufB->BufferArray, BUFFERLENGTH);
	}

	DmaChnClrEvFlags(DMA_CHANNEL2, DMA_EV_ALL_EVNTS);
	DmaChnClrIntFlag(DMA_CHANNEL2);
	mDmaChnClrIntFlag(2);
}


// Here we handle the events being generated by our UART TX DMA channel.

void __ISR(_DMA3_VECTOR) DmaHandler3(void)
{
	DmaChnClrEvFlags(DMA_CHANNEL3, DMA_EV_BLOCK_DONE);
	INTClearFlag(INT_SOURCE_DMA(DMA_CHANNEL3));
}

void __ISR(_DMA_4_VECTOR) DmaHandler4(void)
{
	//currentBuffer will be set to the opposite buffer than the one which
	//needs to have this appended to it.
	if (currentBuffer == BUFFER_A) {
		BufA->BufferArray[BUFFERLENGTH] = 'E';
		BufA->BufferArray[BUFFERLENGTH + 1] = 'N';
		BufA->BufferArray[BUFFERLENGTH + 2] = 'D';
		BufA->BufferArray[BUFFERLENGTH + 3] = '*';
		BufA->BufferArray[BUFFERLENGTH + 4] = (crc & 0xFF000000) >> 24;
		BufA->BufferArray[BUFFERLENGTH + 5] = (crc & 0x00FF0000) >> 16;
		BufA->BufferArray[BUFFERLENGTH + 6] = (crc & 0x0000FF00) >> 8;
		BufA->BufferArray[BUFFERLENGTH + 7] = (crc & 0x000000FF);

		//BufferToUART_TransferA(BUFFERLENGTH + END_MESSAGE);
		BufferToPMP_TransferA(BUFFERLENGTH + END_MESSAGE);

		currentBuffer = BUFFER_B;

	} else if (currentBuffer == BUFFER_B) {
		BufB->BufferArray[BUFFERLENGTH] = 'E';
		BufB->BufferArray[BUFFERLENGTH + 1] = 'N';
		BufB->BufferArray[BUFFERLENGTH + 2] = 'D';
		BufB->BufferArray[BUFFERLENGTH + 3] = '*';
		BufB->BufferArray[BUFFERLENGTH + 4] = (crc & 0xFF000000) >> 24;
		BufB->BufferArray[BUFFERLENGTH + 5] = (crc & 0x00FF0000) >> 16;
		BufB->BufferArray[BUFFERLENGTH + 6] = (crc & 0x0000FF00) >> 8;
		BufB->BufferArray[BUFFERLENGTH + 7] = (crc & 0x000000FF);

		//BufferToUART_TransferB(BUFFERLENGTH + END_MESSAGE);
		BufferToPMP_TransferB(BUFFERLENGTH + END_MESSAGE);

		currentBuffer = BUFFER_A;
	}

	DmaChnClrEvFlags(DMA_CHANNEL4, DMA_EV_ALL_EVNTS);
	DmaChnClrIntFlag(DMA_CHANNEL4);
	mDmaChnClrIntFlag(4);
}

void __ISR(_DMA_5_VECTOR) DmaHandler5(void)
{
	DmaChnClrEvFlags(DMA_CHANNEL5, DMA_EV_BLOCK_DONE);
	INTClearFlag(INT_SOURCE_DMA(DMA_CHANNEL5));
}

//returns which buffer we are currently DMAing into
uint8_t CurrentBuffer(void)
{
        return currentBuffer;
}
