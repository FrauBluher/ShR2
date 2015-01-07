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

static uint8_t DmaTxIntFlag = 1; // flag used in interrupts, signal that DMA transfer ended
static uint8_t inited = 0;
SampleBuffer *BufA;
SampleBuffer *BufB;
DmaChannel chn;

int BufferToUART_Init(SampleBuffer *BufferA, SampleBuffer *BufferB)
{
	if (!inited) {
		BufA = BufferA;
		BufB = BufferB;

		BufA->index = 0;
		BufB->index = 0;

		UARTConfigure(UART1, UART_ENABLE_PINS_TX_RX_ONLY);
		UARTSetLineControl(UART1, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
		UARTSetDataRate(UART1, GetPeripheralClock(), 115200);
		UARTEnable(UART1, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));

		chn = DMA_CHANNEL1; // DMA channel to use for our example

		DmaChnOpen(chn, DMA_CHN_PRI2, DMA_OPEN_DEFAULT);

		//DMA continues transfer after TX FIFO is emptied.
		DmaChnSetEventControl(chn, DMA_EV_START_IRQ_EN | DMA_EV_START_IRQ(_UART1_TX_IRQ));

		INTEnable(INT_SOURCE_DMA(chn), INT_ENABLED); // enable the chn interrupt in the INT controller

		inited = 1;
		return(EXIT_SUCCESS);
	} else {
		return(EXIT_FAILURE);
	}
}

int BufferToSpi_Init(SampleBuffer *BufferA, SampleBuffer *BufferB)
{
	if (!inited) {
		BufA = BufferA;
		BufB = BufferB;

		BufA->index = 0;
		BufB->index = 0;

		// open and configure the SPI channel to use: master, no frame mode, 8 bit mode.
		// we'll be using 80MHz/1 = 80MHz SPI peripheral base clock
		SpiChnOpen(SPI_CHANNEL1, SPI_OPEN_SLVEN | SPI_OPEN_SMP_END |
			SPI_OPEN_CKP_HIGH | SPI_OPEN_MODE8, 1);

		chn = DMA_CHANNEL1; // DMA channel to use for our example

		DmaChnOpen(chn, DMA_CHN_PRI2, DMA_OPEN_DEFAULT);

		//DMA continues transfer after TX FIFO is emptied.
		DmaChnSetEventControl(chn, DMA_EV_START_IRQ_EN | DMA_EV_START_IRQ(_SPI1_TX_IRQ));

		INTEnable(INT_SOURCE_DMA(chn), INT_ENABLED); // enable the chn interrupt in the INT controller

		inited = 1;
		return(EXIT_SUCCESS);
	} else {
		return(EXIT_FAILURE);
	}
}

int BufferToSpi_TransferA(uint16_t transferSize)
{
	// set the transfer:
	// source is our buffer, dest is the SPI transmit buffer
	// source size is the whole buffer, destination size is two bytes
	// cell size is two bytes: we want one byte to be sent per each SPI TXBE event
	DmaChnSetTxfer(DMA_CHANNEL1, BufA->BufferArray, (void*) &SPI1BUF, transferSize, 1, 1);

	DmaChnSetEvEnableFlags(chn, DMA_EV_BLOCK_DONE); // enable the transfer done interrupt: pattern match or all the characters transferred

	DmaChnStartTxfer(chn, DMA_WAIT_NOT, 0); // force the DMA transfer: the UART1 tx flag it's already been active
}

int BufferToSpi_TransferB(uint16_t transferSize)
{
	// set the transfer:
	// source is our buffer, dest is the SPI transmit buffer
	// source size is the whole buffer, destination size is two bytes
	// cell size is two bytes: we want one byte to be sent per each SPI TXBE event
	DmaChnSetTxfer(DMA_CHANNEL1, BufB->BufferArray, (void*) &SPI1BUF, transferSize, 1, 1);

	DmaChnSetEvEnableFlags(chn, DMA_EV_BLOCK_DONE); // enable the transfer done interrupt: pattern match or all the characters transferred

	DmaChnStartTxfer(chn, DMA_WAIT_NOT, 0); // force the DMA transfer: the UART1 tx flag it's already been active
}

uint8_t BufferToUART_TransferA(uint16_t transferSize)
{
	// set the transfer source and dest addresses, source and dest size and cell size
	DmaChnSetTxfer(DMA_CHANNEL1, BufA->BufferArray, (void*) &U1TXREG, transferSize, 1, 1);

	DmaChnSetEvEnableFlags(chn, DMA_EV_BLOCK_DONE); // enable the transfer done interrupt: pattern match or all the characters transferred

	DmaChnStartTxfer(chn, DMA_WAIT_NOT, 0); // force the DMA transfer: the UART1 tx flag it's already been active
}

uint8_t BufferToUART_TransferB(uint16_t transferSize)
{
	DmaChnSetTxfer(DMA_CHANNEL1, BufB->BufferArray, (void*) &U1TXREG, transferSize, 1, 1);

	DmaChnSetEvEnableFlags(chn, DMA_EV_BLOCK_DONE); // enable the transfer done interrupt: all the characters transferred

	DmaChnStartTxfer(chn, DMA_WAIT_NOT, 0); // force the DMA transfer: the UART1 tx flag it's already been active
}

// handler for the DMA channel 1 interrupt

void __ISR(_DMA1_VECTOR) DmaHandler1(void)
{
	_RF8 = 0;
	INTClearFlag(INT_SOURCE_DMA(DMA_CHANNEL1)); // release the interrupt in the INT controller, we're servicing int
}
