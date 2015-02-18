/**
 * @file	SB_Computation.c
 * @author 	Henry Crute
 * @date	February 2015
 * @brief 	Implimentations of computations on type SampleBuffer
 */

#include <math.h>

#include "DMA_Transfer.h"

#define WINDOW_SIZE 130
#define CHANNEL 1


//takes the RMS of the given SampleBuffer
//must do something about the DC offset on the ADC
//Also must take into account the signed-ness of the data

uint32_t SB_RMS(SampleBuffer *buffer)
{
	uint64_t rawrms = 0;
	uint16_t i;
	for (i = 0; i < WINDOW_SIZE; i++) {
		// TODO: optimize with a single 32 bit load
		uint64_t value =
			buffer->BufferArray[(3*CHANNEL+1) + i * 13] |
			buffer->BufferArray[(3*CHANNEL+2) + i * 13] << 1 |
			buffer->BufferArray[(3*CHANNEL+3) + i * 13] << 2;
		rawrms += value;
	}
	return rawrms;
	//return sqrt(rawrms / WINDOW_SIZE);
}
