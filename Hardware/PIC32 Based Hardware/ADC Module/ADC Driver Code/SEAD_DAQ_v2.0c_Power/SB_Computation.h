/**
 * @file	SB_Computation.h
 * @author 	Henry Crute
 * @date	February 2015
 * @brief 	Provides methods to do computations on type SampleBuffer
 */

#ifndef SB_COMPUTATION_H
#define	SB_COMPUTATION_H

#include "DMA_Transfer.h"

uint32_t SB_RMS(SampleBuffer *);
int32_t SB_AVG(SampleBuffer *);
int32_t SB_VOL(int32_t);

#endif	/* SB_COMPUTATION_H */

