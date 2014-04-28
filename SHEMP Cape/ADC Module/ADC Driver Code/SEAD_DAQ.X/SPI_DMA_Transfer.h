//TODO: ADD DOCUMENTATION AND LICENSE

#ifndef SPI_DMA_TRANSFER_H
#define	SPI_DMA_TRANSFER_H

#include <stdint.h>

typedef struct {
    int16_t BufferArray[30000];
    uint8_t bufferFull;
    uint8_t txStarted;
} SampleBuffer;

int BufferToSpi_Init(SampleBuffer *BufferA, SampleBuffer *BufferB);
int BufferToSpi_Transfer(void);

#endif	/* SPI_DMA_TRANSFER_H */

