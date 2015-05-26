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
//should not exceed 2^15 32768 to prevent overflow
#define WINDOW_SIZE 976//2

//the channel to do the calculations
#define CHANNEL 0

//in microvolts
#define V_REF 1200000

//2^24
#define MAX_24BIT 0x1000000

#define RMS_BASE_SCALER 1.2 /(1<<24 * 1<<PGA_CH1_CONF)
#define RMS_SCALER RMS_BASE_SCALER*RMS_BASE_SCALER/WINDOW_SIZE
#define WATTAGE_SCALER 1.309 * 120 * 20 / 0.333

//takes the RMS of the given SampleBuffer
//must do something about the DC offset on the ADC
//Also must take into account the signed-ness of the data

uint32_t SB_RMS(SampleBuffer *buffer)
{
	uint64_t rawrms = 0;
	uint32_t i;

	for (i = 0; i < WINDOW_SIZE; i++) {
		// TODO: optimize with a single 32 bit load
		int64_t value =
			buffer->BufferArray[(3*CHANNEL+0) + i * 13] | // this should be +0 for channel 0 +1 for others
			buffer->BufferArray[(3*CHANNEL+2) + i * 13] << 16 |
			buffer->BufferArray[(3*CHANNEL+3) + i * 13] << 8;
		value = ((value << 40) >> 40); // sign extends
		rawrms += (value * value);
	}
	// PGA_CH1_CONF
	float x = sqrt(rawrms * RMS_SCALER);
	// float rms = x * s / sqrt(WINDOW_SIZE);
	//float arms = 1.309 * 1000 * x * 20 / 0.333;
	float watts = x * WATTAGE_SCALER;
	return (uint32_t)watts;
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
//STUB!
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