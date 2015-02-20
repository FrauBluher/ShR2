/**
 * @file	SB_Computation.c
 * @author 	Henry Crute
 * @date	February 2015
 * @brief 	Implimentations of computations on type SampleBuffer
 */

#include <math.h>

#include "DMA_Transfer.h"
#include "MCP391x.h"

//10 cycles at sample speed 3906.25/sec
#define WINDOW_SIZE 1562

//the channel to do the calculations
#define CHANNEL 1

#define V_REF 1.2


//takes the RMS of the given SampleBuffer
//must do something about the DC offset on the ADC
//Also must take into account the signed-ness of the data

uint32_t SB_RMS(SampleBuffer *buffer)
{
	uint64_t rawrms = 0;
	uint16_t i;
	for (i = 0; i < WINDOW_SIZE; i++) {
		// TODO: optimize with a single 32 bit load
		uint32_t value =
			buffer->BufferArray[(3*CHANNEL+1) + i * 13] |
			buffer->BufferArray[(3*CHANNEL+2) + i * 13] << 16 |
			buffer->BufferArray[(3*CHANNEL+3) + i * 13] << 8;
		value = ((value << 8) >> 8); // sign extend
		rawrms += value * value;
	}
	return sqrt(rawrms / WINDOW_SIZE);
}

//takes values and averages them accumulator style.
int32_t SB_AVG(SampleBuffer *buffer)
{
	int64_t accumulator = 0;
	uint16_t i;
	for (i = 0; i < WINDOW_SIZE; i++) {
		// TODO: optimize with a single 32 bit load
		int32_t value =
			buffer->BufferArray[(3*CHANNEL+1) + i * 13] |
			buffer->BufferArray[(3*CHANNEL+2) + i * 13] << 16 |
			buffer->BufferArray[(3*CHANNEL+3) + i * 13] << 8;
		value = ((value << 8) >> 8); // sign extend
		accumulator += value;
	}
	return (accumulator / WINDOW_SIZE);
}


//returns the volts in millivolts depending on the PGA setting
//of the input number
int32_t SB_VOL(int32_t input)
{
	switch (CHANNEL) {
	case 0:
		return (input);
		break;
	case 1:
		return (input);
		break;
	case 2:
		return (input);
		break;
	case 3:
		return (input);
		break;
	default:
		return (input);
		break;
	}
}