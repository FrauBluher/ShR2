/**
 * @file	SB_Computation.c
 * @author 	Henry Crute
 * @date	February 2015
 * @brief 	Implimentations of computations on type SampleBuffer
 */

#include <math.h>

#include "DMA_Transfer.h"


//takes the RMS of the given SampleBuffer
//must do something about the DC offset on the ADC
//Also must take into account the signed-ness of the data
//also this is a 24bit adc and we are reading in unsigned bytes

uint8_t SB_RMS(SampleBuffer *buffer) {
        uint32_t rawrms = 0;
        uint32_t rms = 0;
        for (uint8_t i = 0; i < 130; i++) {
                rawrms += buffer->BufferArray[i] * buffer->BufferArray[i];
        }
        return sqrt(rawrms / 130);
}
