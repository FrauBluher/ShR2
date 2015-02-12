/**
 * @file	SB_Computation.c
 * @author 	Henry Crute
 * @date	February 2015
 * @brief 	Implimentations of computations on type SampleBuffer
 */

#include "DMA_Transfer.h"


//takes the RMS of the given SampleBuffer
uint8_t SB_RMS(SampleBuffer *buffer) {
    return buffer->BufferArray[0];
}
