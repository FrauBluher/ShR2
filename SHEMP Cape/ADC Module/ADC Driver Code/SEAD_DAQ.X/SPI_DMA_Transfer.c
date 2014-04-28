//TODO: ADD DOCUMENTATION AND LICENSE

#include <xc.h>
#include "SPI_DMA_Transfer.h"
#include <plib.h>

static uint8_t DmaTxIntFlag = 1; // flag used in interrupts, signal that DMA transfer ended
static uint8_t inited = 0;
SampleBuffer *BufA;
SampleBuffer *BufB;

int BufferToSpi_Init(SampleBuffer *BufferA, SampleBuffer *BufferB)
{
	if (!inited) {
		BufA = BufferA;
		BufB = BufferB;

		DmaChannel dmaTxChn = DMA_CHANNEL1; // DMA channel to use for our example
		SpiChannel spiTxChn = SPI_CHANNEL1; // the transmitting SPI channel to use

		// open and configure the SPI channel to use: master, no frame mode, 16 bit mode.
		// won't use SS for communicating with the slave
		// we'll be using 40MHz/2=20MHz SPI clock
		SpiChnOpen(spiTxChn, SPI_OPEN_MSTEN | SPI_OPEN_SMP_END | SPI_OPEN_MODE16, 2);

		// open and configure the DMA channel.
		DmaChnOpen(dmaTxChn, DMA_CHN_PRI2, DMA_OPEN_DEFAULT);

		// set the events: we want the SPI transmit buffer empty interrupt to start our transfer
		DmaChnSetEventControl(dmaTxChn, DMA_EV_START_IRQ_EN | DMA_EV_START_IRQ(_SPI1_TX_IRQ));


		//DmaChnSetTxfer(dmaTxChn, BufferA->BufferArray, (void*) &SPI1BUF, sizeof (BufferA->BufferArray), 2, 2);

		DmaChnSetEvEnableFlags(dmaTxChn, DMA_EV_BLOCK_DONE); //enable the transfer done interrupt, when all buffer transferred

		INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
		INTEnableInterrupts();

		INTSetVectorPriority(INT_VECTOR_DMA(dmaTxChn), INT_PRIORITY_LEVEL_5); // set INT controller priority
		INTSetVectorSubPriority(INT_VECTOR_DMA(dmaTxChn), INT_SUB_PRIORITY_LEVEL_3); // set INT controller sub-priority

		INTEnable(INT_SOURCE_DMA(dmaTxChn), INT_ENABLED); // enable the chn interrupt in the INT controller

		// Return stuff here...
		return(EXIT_SUCCESS);
	} else {
		return(EXIT_FAILURE);
	}
}

int BufferToSpi_Transfer(void)
{
	if (inited) {
		if (BufA->bufferFull && !BufB->bufferFull && !BufA->txStarted) {
			// set the transfer:
			// source is our buffer, dest is the SPI transmit buffer
			// source size is the whole buffer, destination size is two bytes
			// cell size is two bytes: we want two byte to be sent per each SPI TXBE event
			DmaChnSetTxfer(DMA_CHANNEL1, BufA->BufferArray, (void*) &SPI1BUF, sizeof(BufA->BufferArray), 2, 2);

			BufA->txStarted = 1;
			DmaTxIntFlag = 0;
			DmaChnStartTxfer(DMA_CHANNEL1, DMA_WAIT_NOT, 0); // force the DMA transfer: the SPI TBE flag it's already been active

			return(EXIT_SUCCESS);
		} else if (!BufA->bufferFull && BufB->bufferFull && !BufB->txStarted) {
			// set the transfer:
			// source is our buffer, dest is the SPI transmit buffer
			// source size is the whole buffer, destination size is two bytes
			// cell size is two bytes: we want two byte to be sent per each SPI TXBE event
			DmaChnSetTxfer(DMA_CHANNEL1, BufB->BufferArray, (void*) &SPI1BUF, sizeof(BufB->BufferArray), 2, 2);

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

// handler for the DMA channel 1 interrupt

void __ISR(_DMA1_VECTOR, ipl5) DmaHandler1(void)
{
	int evFlags; // event flags when getting the interrupt

	INTClearFlag(INT_SOURCE_DMA(DMA_CHANNEL1)); // acknowledge the INT controller, we're servicing int

	evFlags = DmaChnGetEvFlags(DMA_CHANNEL1); // get the event flags

	if (BufB->txStarted) {
		BufB->bufferFull = 0;
		BufB->txStarted = 0;
	} else if (BufA->txStarted) {
		BufA->bufferFull = 0;
		BufA->txStarted = 0;
	}

	if (evFlags & DMA_EV_BLOCK_DONE) { // just a sanity check. we enabled just the DMA_EV_BLOCK_DONE transfer done interrupt
		DmaTxIntFlag = 1;
		DmaChnClrEvFlags(DMA_CHANNEL1, DMA_EV_BLOCK_DONE);
	}
}