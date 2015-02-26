/**
 * @file	SB_Computation.c
 * @author 	Henry Crute
 * @date	February 2015
 * @brief 	Implimentations of computations on type SampleBuffer
 */

#include <math.h>

#include "SB_Computation.h"
#include "DMA_Transfer.h"
#include "MCP391x.h"

//10 cycles at sample speed 3906.25/sec
#define WINDOW_SIZE 1562

//the channel to do the calculations
#define CHANNEL 1

//in microvolts
#define V_REF 1200000

//2^24
#define MAX_24BIT 0x1000000


//takes the RMS of the given SampleBuffer
//must do something about the DC offset on the ADC
//Also must take into account the signed-ness of the data

uint32_t SB_RMS(SampleBuffer *buffer)
{
	float rawrms = 0;
	uint16_t i;
	//TODO find the the most efficient way to scale
	float s = 1.2 /(1<<24 * 1<<PGA_CH1_CONF);
	for (i = 0; i < WINDOW_SIZE; i++) {
		// TODO: optimize with a single 32 bit load
		int64_t value =
			buffer->BufferArray[(3*CHANNEL+1) + i * 13] | // this should be +0 for channel 0
			buffer->BufferArray[(3*CHANNEL+2) + i * 13] << 16 |
			buffer->BufferArray[(3*CHANNEL+3) + i * 13] << 8;
		value = ((value << 40) >> 40); // sign extend
		float v = value * s;
		rawrms += (v * v);
	}
	// PGA_CH1_CONF
	float x = sqrt(rawrms / WINDOW_SIZE);
	// float rms = x * s / sqrt(WINDOW_SIZE);
	float arms = 1000 * x * 20 / 0.333;
	return (uint32_t)arms;
}

//takes values and averages them accumulator style
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


//returns the volts in microvolts depending on the PGA setting
//of the input number
uint32_t SB_VOL(void)
{
	int64_t range;
	switch (CHANNEL) {
	case 0:
		break;
	case 1:
		//mostly redundant computations, but whatever
		range = V_REF / (MAX_24BIT * pow(2, PGA_CH1_CONF));
			//(MAX_24BIT * pow(2, PGA_CH0_CONF));
		break;
	case 2:
		break;
	case 3:
		break;
	default:
		range = 0;
		break;
	}
	return (range);
}