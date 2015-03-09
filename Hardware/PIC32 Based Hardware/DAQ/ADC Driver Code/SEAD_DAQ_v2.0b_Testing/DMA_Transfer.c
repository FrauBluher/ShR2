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

// some local data
int DmaIntFlag; // flag used in interrupts

int BufferToUART3_Init(SampleBuffer *BufferA, SampleBuffer *BufferB)
{
	char dmaBuff[256 + 1]; // we'll store the received data here
	int chn = 1; // DMA channel to use for our example

	UARTConfigure(UART3, UART_ENABLE_PINS_TX_RX_ONLY);
	UARTSetLineControl(UART3, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
	UARTSetDataRate(UART3, GetPeripheralClock(), 115200);
	UARTEnable(UART3, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));


	// configure the channel
	DmaChnOpen(chn, DMA_CHN_PRI2, DMA_OPEN_MATCH);

	DmaChnSetMatchPattern(chn, '\r'); // set \r as ending character

	// set the events: we want the UART2 rx interrupt to start our transfer
	// also we want to enable the pattern match: transfer stops upon detection of CR
	DmaChnSetEventControl(chn, DMA_EV_START_IRQ_EN | DMA_EV_MATCH_EN | DMA_EV_START_IRQ(_UART3_RX_IRQ));

	// set the transfer source and dest addresses, source and dest sizes and the cell size
	DmaChnSetTxfer(chn, (void*) &U3RXREG, dmaBuff, 1, 256, 1);

	DmaChnSetEvEnableFlags(chn, DMA_EV_BLOCK_DONE); // enable the transfer done interrupt: pattern match or all the characters transferred

	INTEnableSystemMultiVectoredInt(); // enable system wide multi vectored interrupts

	DmaChnSetIntPriority(chn, INT_PRIORITY_LEVEL_5, INT_SUB_PRIORITY_LEVEL_3); // set INT controller priorities

	DmaChnIntEnable(chn); // enable the chn interrupt in the INT controller

	DmaIntFlag = 0; // clear the interrupt flag

	// enable the chn
	DmaChnEnable(chn);

	// let the user know that he has to enter a string
	// for example, you could do something like:
	// printf("\r\nType a string less than 256 characters followed by Enter key...Will echo it using the DMA\r\n\r\n");

	// wait for the data to come in
	while (!DmaIntFlag); // just block here. In a real application you can do some other stuff while the DMA transfer is taking place

	// now the TX part
	// reconfigure the channel
	DmaChnOpen(chn, DMA_CHN_PRI2, DMA_OPEN_MATCH);

	// set the events: now the start event is the UART tx being empty
	// we maintain the pattern match mode
	DmaChnSetEventControl(chn, DMA_EV_START_IRQ_EN | DMA_EV_MATCH_EN | DMA_EV_START_IRQ(_UART3_TX_IRQ));

	// set the transfer source and dest addresses, source and dest size and cell size
	DmaChnSetTxfer(chn, dmaBuff, (void*) &U3TXREG, 256, 1, 1);


	DmaIntFlag = 0; // clear the interrupt flag

	DmaChnStartTxfer(chn, DMA_WAIT_NOT, 0); // force the DMA transfer: the UART2 tx flag it's already been active

	// wait for data to be output
	while (!DmaIntFlag); // just block here

	// DMA Echo is complete

	DmaChnIntDisable(chn); // disable further interrupts from the DMA controller
}

// handler for the DMA channel 1 interrupt

void __ISR(_DMA1_VECTOR, ipl5) DmaHandler1(void)
{
	int evFlags; // event flags when getting the interrupt

	mDmaChnClrIntFlag(1); // release the interrupt in the INT controller, we're servicing int

	evFlags = DmaChnGetEvFlags(1); // get the event flags

	if (evFlags & DMA_EV_BLOCK_DONE) { // just a sanity check. we enabled just the DMA_EV_BLOCK_DONE transfer done interrupt
		DmaIntFlag = 1;
		DmaChnClrEvFlags(1, DMA_EV_BLOCK_DONE);
	}
}