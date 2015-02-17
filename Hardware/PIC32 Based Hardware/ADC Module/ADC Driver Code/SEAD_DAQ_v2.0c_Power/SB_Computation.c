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

uint32_t SB_RMS(SampleBuffer *buffer) {
        uint32_t rawrms = 0;
        uint16_t i;
        for (i = 0; i < 130; i++) {
                rawrms += *(buffer->BufferArray + i) * *(buffer->BufferArray + i);
        }
        return sqrt(rawrms / 130);
}
