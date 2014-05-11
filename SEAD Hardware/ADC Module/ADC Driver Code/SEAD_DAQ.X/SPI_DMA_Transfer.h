//TODO: ADD DOCUMENTATION AND LICENSE

#ifndef SPI_DMA_TRANSFER_H
#define	SPI_DMA_TRANSFER_H
#define MAX32

#include <stdint.h>
#define BUFFERLENGTH 10000

//8 bit integer should be changed to 16 bits for SPI testing.
typedef struct {
    int8_t BufferArray[BUFFERLENGTH];
    uint8_t bufferFull;
    uint8_t txStarted;
    uint16_t index;
} SampleBuffer;

int BufferToUART_Init(SampleBuffer *BufferA, SampleBuffer *BufferB);
int BufferToSpi_Init(SampleBuffer *BufferA, SampleBuffer *BufferB);
int BufferToSpi_Transfer(void);
uint8_t BufferToUART_TransferA(uint16_t transferSize);
uint8_t BufferToUART_TransferB(uint16_t transferSize);

#endif	/* SPI_DMA_TRANSFER_H */

