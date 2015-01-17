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
SampleBuffer *BufA;
SampleBuffer *BufB;
DmaChannel chn;
DmaChannel rxChn;
uint8_t dmaBuff[256] = {};

uint8_t BufferToUART_Init(void)
{
	UARTConfigure(UART3, UART_ENABLE_PINS_TX_RX_ONLY | UART_ENABLE_HIGH_SPEED);
	UARTSetLineControl(UART3, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
	UARTSetDataRate(UART3, GetPeripheralClock(), 1152000);
	UARTEnable(UART3, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));

	chn = DMA_CHANNEL3; // DMA channel to use for our example

	DmaChnOpen(chn, DMA_CHN_PRI2, DMA_OPEN_DEFAULT);

	//DMA continues transfer after TX FIFO is emptied.
	DmaChnSetEventControl(chn, DMA_EV_START_IRQ_EN | DMA_EV_START_IRQ(_UART3_TX_IRQ));

	INTEnable(INT_SOURCE_DMA(chn), INT_ENABLED); // enable the chn interrupt in the INT controller

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

		chn = DMA_CHANNEL1;
		rxChn = DMA_CHANNEL2;

		DmaChnOpen(chn, DMA_CHN_PRI2, DMA_OPEN_DEFAULT);
		DmaChnOpen(rxChn, DMA_CHN_PRI3, DMA_OPEN_DEFAULT);

		//DMA continues transfer after TX FIFO is emptied.
		DmaChnSetEventControl(chn, DMA_EV_START_IRQ(_SPI1_TX_IRQ));
		DmaChnSetEventControl(rxChn, DMA_EV_START_IRQ(_SPI1_RX_IRQ));

		DmaChnSetEvEnableFlags(chn, DMA_EV_BLOCK_DONE);
		DmaChnSetEvEnableFlags(rxChn, DMA_EV_BLOCK_DONE | DMA_EV_DST_FULL);

		DmaChnClrEvFlags(chn, DMA_EV_ALL_EVNTS);
		DmaChnClrEvFlags(rxChn, DMA_EV_ALL_EVNTS);

		DmaChnSetIntPriority(rxChn, 5, 3);
		DmaChnIntEnable(rxChn);
		DmaChnClrIntFlag(rxChn);


		inited = 1;
		return(EXIT_SUCCESS);
	} else {
		return(EXIT_FAILURE);
	}
}

void StartSPIAcquisition(uint8_t buffer)
{
	SPI1STAT = 0; //So we don't lose events.

	if (buffer == BUFFER_A) {
		DmaChnSetTxfer(rxChn, (void *) &SPI1BUF, BufA->BufferArray, 1, BUFFERLENGTH, 1);
	} else if (buffer == BUFFER_B) {
		DmaChnSetTxfer(rxChn, (void *) &SPI1BUF, BufB->BufferArray, 1, BUFFERLENGTH, 1);
	}
	DmaChnEnable(rxChn);
}

void DMAStartUARTRX(void)
{
	DmaChnOpen(DMA_CHANNEL0, DMA_CHN_PRI1, DMA_OPEN_MATCH); //High priority for control.

	DmaChnSetMatchPattern(DMA_CHANNEL0, '\r');

	DmaChnSetEventControl(DMA_CHANNEL0, DMA_EV_START_IRQ_EN | DMA_EV_MATCH_EN | DMA_EV_START_IRQ(_UART3_RX_IRQ));

	DmaChnSetTxfer(DMA_CHANNEL0, (void*) &U3RXREG, dmaBuff, 1, 256, 1);

	DmaChnSetEvEnableFlags(DMA_CHANNEL0, DMA_EV_BLOCK_DONE);

	DmaChnSetIntPriority(DMA_CHANNEL0, INT_PRIORITY_LEVEL_5, INT_SUB_PRIORITY_LEVEL_3);
	DmaChnIntEnable(DMA_CHANNEL0);
	DmaChnEnable(DMA_CHANNEL0);
}

uint8_t BufferToSpi_Transfer(uint8_t *txBuffer, uint16_t transferSize)
{
	DmaChnSetTxfer(DMA_CHANNEL1, txBuffer, (void*) &SPI1BUF, transferSize, 1, 1);

	DmaChnStartTxfer(DMA_CHANNEL1, DMA_WAIT_NOT, 0);
}

uint8_t BufferToUART_TransferA(uint16_t transferSize)
{
	// set the transfer source and dest addresses, source and dest size and cell size
	DmaChnSetTxfer(DMA_CHANNEL3, BufA->BufferArray, (void*) &U3TXREG, transferSize, 1, 1);

	DmaChnSetEvEnableFlags(DMA_CHANNEL3, DMA_EV_BLOCK_DONE); // enable the transfer done interrupt: pattern match or all the characters transferred

	DmaChnStartTxfer(DMA_CHANNEL3, DMA_WAIT_NOT, 0); // force the DMA transfer: the UART1 tx flag it's already been active
}

uint8_t BufferToUART_TransferB(uint16_t transferSize)
{
	DmaChnSetTxfer(DMA_CHANNEL3, BufB->BufferArray, (void*) &U3TXREG, transferSize, 1, 1);

	DmaChnSetEvEnableFlags(DMA_CHANNEL3, DMA_EV_BLOCK_DONE); // enable the transfer done interrupt: all the characters transferred

	DmaChnStartTxfer(DMA_CHANNEL3, DMA_WAIT_NOT, 0);
}

void __ISR(_DMA0_VECTOR) DmaHandler0(void)
{
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
	if (currentBuffer == BUFFER_A) {

		StartSPIAcquisition(BUFFER_B);

		BufferToUART_TransferA(BUFFERLENGTH);

		currentBuffer = BUFFER_B;

	} else if (currentBuffer == BUFFER_B) {

		StartSPIAcquisition(BUFFER_A);

		BufferToUART_TransferB(BUFFERLENGTH);

		currentBuffer = BUFFER_A;

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
