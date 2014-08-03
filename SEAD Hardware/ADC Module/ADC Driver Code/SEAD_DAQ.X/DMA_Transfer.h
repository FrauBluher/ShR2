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
 * @file	DMA_Transfer.h
 * @author 	Pavlo Milo Manovi
 * @date	April, 2014
 * @brief 	This library provides methods to send data over UART and SPI using
 *              non-blocking DMA.  See notes in the .c file for specifics and future
 *              required work.
 */

#ifndef SPI_DMA_TRANSFER_H
#define	SPI_DMA_TRANSFER_H
#define MAX32

#include <stdint.h>
#define BUFFERLENGTH 8192

//8 bit integer should be changed to 16 bits for SPI testing.
typedef struct {
    uint8_t BufferArray[BUFFERLENGTH];
    uint8_t bufferFull;
    uint8_t txStarted;
    uint16_t index;
} SampleBuffer;

int BufferToUART_Init(SampleBuffer *BufferA, SampleBuffer *BufferB);
int BufferToSpi_Init(SampleBuffer *BufferA, SampleBuffer *BufferB);
int BufferToSpi_TransferA(uint16_t transferSize);
int BufferToSpi_TransferB(uint16_t transferSize);
uint8_t BufferToUART_TransferA(uint16_t transferSize);
uint8_t BufferToUART_TransferB(uint16_t transferSize);

#endif	/* SPI_DMA_TRANSFER_H */

