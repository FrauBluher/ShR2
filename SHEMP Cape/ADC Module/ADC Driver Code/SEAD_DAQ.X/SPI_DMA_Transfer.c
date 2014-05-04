//TODO: ADD DOCUMENTATION AND LICENSE
//TODO: RENAME TO DMA_Transfer.c/.h

#include <xc.h>
#include "SPI_DMA_Transfer.h"
#include <plib.h>

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
		//Fill this in once you get UART working.
		//Deleted code that might work.

		inited = 1;
		return(EXIT_SUCCESS);
	} else {
		return(EXIT_FAILURE);
	}
}

int BufferToSpi_Transfer(void)
{
	if (inited) {
		if (!BufB->bufferFull && !BufA->txStarted) {
			// set the transfer:
			// source is our buffer, dest is the SPI transmit buffer
			// source size is the whole buffer, destination size is two bytes
			// cell size is two bytes: we want two byte to be sent per each SPI TXBE event
#ifdef MAX32
			DmaChnSetTxfer(DMA_CHANNEL1, BufA->BufferArray, (void*) &U1TXREG, sizeof(BufA->BufferArray), 1, 1);
#endif
#ifndef MAX32
			DmaChnSetTxfer(DMA_CHANNEL1, BufA->BufferArray, (void*) &SPI1BUF, sizeof(BufA->BufferArray), 2, 2);
#endif

			BufA->txStarted = 1;
			BufA->bufferFull = 0;
			DmaTxIntFlag = 0;
			DmaChnStartTxfer(DMA_CHANNEL1, DMA_WAIT_NOT, 0); // force the DMA transfer: the SPI TBE flag it's already been active

			return(EXIT_SUCCESS);
		} else if (!BufA->bufferFull && BufB->bufferFull && !BufB->txStarted) {
			// set the transfer:
			// source is our buffer, dest is the SPI transmit buffer
			// source size is the whole buffer, destination size is two bytes
			// cell size is two bytes: we want two byte to be sent per each SPI TXBE event
#ifdef MAX32
			DmaChnSetTxfer(DMA_CHANNEL1, BufB->BufferArray, (void*) &U1TXREG, sizeof(BufB->BufferArray), 1, 1);
#endif
#ifndef MAX32
			DmaChnSetTxfer(DMA_CHANNEL1, BufB->BufferArray, (void*) &SPI1BUF, sizeof(BufB->BufferArray), 2, 2);
#endif

			BufB->txStarted = 1;
			DmaTxIntFlag = 0;
			DmaChnStartTxfer(DMA_CHANNEL1, DMA_WAIT_NOT, 0); // force the DMA transfer: the SPI TBE flag it's already been active

			return(EXIT_SUCCESS);
		} else {
			return(EXIT_FAILURE);
		}
	} else {
		return(EXIT_FAILURE);
	}

}

uint8_t BufferToUART_TransferA(uint16_t transferSize)
{
	// set the transfer source and dest addresses, source and dest size and cell size
	DmaChnSetTxfer(DMA_CHANNEL1, BufA->BufferArray, (void*) &U1TXREG, transferSize, 1, 1);

	DmaChnSetEvEnableFlags(chn, DMA_EV_BLOCK_DONE); // enable the transfer done interrupt: pattern match or all the characters transferred

	//INTEnable(INT_SOURCE_DMA(chn), INT_ENABLED); // enable the chn interrupt in the INT controller

	DmaChnStartTxfer(chn, DMA_WAIT_NOT, 0); // force the DMA transfer: the UART1 tx flag it's already been active

	//INTEnable(INT_SOURCE_DMA(chn), INT_DISABLED); // disable further interrupts from the DMA controller
}

uint8_t BufferToUART_TransferB(uint16_t transferSize)
{
	// set the transfer source and dest addresses, source and dest size and cell size
	DmaChnSetTxfer(DMA_CHANNEL1, BufB->BufferArray, (void*) &U1TXREG, transferSize, 1, 1);

	DmaChnSetEvEnableFlags(chn, DMA_EV_BLOCK_DONE); // enable the transfer done interrupt: all the characters transferred

	//INTEnable(INT_SOURCE_DMA(chn), INT_ENABLED); // enable the chn interrupt in the INT controller

	DmaChnStartTxfer(chn, DMA_WAIT_NOT, 0); // force the DMA transfer: the UART1 tx flag it's already been active

	//INTEnable(INT_SOURCE_DMA(chn), INT_DISABLED); // disable further interrupts from the DMA controller
}

// handler for the DMA channel 1 interrupt

void __ISR(_DMA1_VECTOR, ipl5) DmaHandler1(void)
{
	int evFlags; // event flags when getting the interrupt

	INTClearFlag(INT_SOURCE_DMA(DMA_CHANNEL1)); // release the interrupt in the INT controller, we're servicing int

//	evFlags = DmaChnGetEvFlags(DMA_CHANNEL1); // get the event flags
//
//	if (evFlags & DMA_EV_BLOCK_DONE) { // just a sanity check. we enabled just the DMA_EV_BLOCK_DONE transfer done interrupt
//		if (BufA->txStarted) {
//			BufA->txStarted = 0;
//		} else {
//			BufB->txStarted = 0;
//		}
//		//DmaChnClrEvFlags(DMA_CHANNEL1, DMA_EV_BLOCK_DONE);
//	}
}
