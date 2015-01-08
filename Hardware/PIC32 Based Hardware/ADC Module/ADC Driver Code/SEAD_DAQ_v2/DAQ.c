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
 * @file	DAQ.c
 * @author 	Pavlo Milo Manovi
 * @date	April, 2014
 * @brief 	Main loop for the DAQ.
 */


#include "ADCModuleBoard.h"
#include <plib.h>

SampleBuffer BufferA;
SampleBuffer BufferB;

static uint8_t flag = 0;

void InitFSM(void);

enum {
	DAQ_INIT,
	DAQ_GET_DATA,
	DAQ_SAMPLES_TO_BUFFER_A,
	DAQ_SAMPLES_TO_BUFFER_B,
	DAQ_SEND_BUFFER_A,
	DAQ_SEND_BUFFER_B,
	DAQ_WAIT_FOR_CONVERSION,
	DAQ_WAIT,
	DAQ_FATAL_ERROR,
	BUFFER_A,
	BUFFER_B
};

typedef struct {
	uint8_t currentBuffer;
	uint8_t nextState;
	uint8_t generalFlag;
} DAQFSMInfo;

static MCP391x_Info ADCInfo;
static DAQFSMInfo FSMInfo;

int main(void)
{
	InitFSM();
	ADCModuleBoard_Init(&BufferA, &BufferB, &ADCInfo);
	TRISBbits.TRISB6 = 0;
	LATBbits.LATB6 = 1;
	
	while (1) {


		/*
				switch (FSMInfo.nextState) {
				case DAQ_INIT:
					if (ADCModuleBoard_Init(&BufferA, &BufferB, &ADCInfo) == EXIT_SUCCESS) {
						flag = 1;
						FSMInfo.nextState = DAQ_GET_DATA;
						TRISBbits.TRISB6 = 0;
						LATBbits.LATB6 = 1;
					} else {
						FSMInfo.nextState = DAQ_FATAL_ERROR;
					}
					break;

				case DAQ_GET_DATA:

					FSMInfo.nextState = DAQ_WAIT_FOR_CONVERSION;
					break;

				case DAQ_SAMPLES_TO_BUFFER_A:
					if (BufferA.index < BUFFERLENGTH - 1) {
						//ADS85x8_GetSamples();
						if (ADCInfo.newData) {
		//					BufferA.BufferArray[BufferA.index] = (ADCInfo.sampledDataChA0 & 0xFF00) >> 8;
		//					BufferA.index++;
		//					BufferA.BufferArray[BufferA.index] = (ADCInfo.sampledDataChA0 & 0x00FF);
		//					BufferA.index++;

							FSMInfo.nextState = DAQ_GET_DATA;
						} else {
							FSMInfo.nextState = DAQ_FATAL_ERROR;
						}
					} else {
						FSMInfo.nextState = DAQ_SEND_BUFFER_A;
					}
					break;

				case DAQ_SAMPLES_TO_BUFFER_B:
					//For UART we split the 16 bit long value into two bytes...
					if (BufferB.index < BUFFERLENGTH - 1) {
						//ADS85x8_GetSamples();
						if (ADCInfo.newData) {

							//					BufferB.BufferArray[BufferB.index] = (ADCInfo.sampledDataChA0 & 0xFF00) >> 8;
							//					BufferB.index++;
							//					BufferB.BufferArray[BufferB.index] = (ADCInfo.sampledDataChA0 & 0x00FF);
							//					BufferB.index++;

							FSMInfo.nextState = DAQ_GET_DATA;
						} else {
							FSMInfo.nextState = DAQ_FATAL_ERROR;
						}
					} else {
						FSMInfo.nextState = DAQ_SEND_BUFFER_B;
					}
					break;

				case DAQ_SEND_BUFFER_A:
					if (DmaChnGetEvFlags(DMA_CHANNEL1) & DMA_EV_BLOCK_DONE || flag) {
						flag = 0;
						BufferToSpi_TransferA(BufferA.index);
						BufferA.index = 0;
						FSMInfo.currentBuffer = BUFFER_B;
						FSMInfo.nextState = DAQ_GET_DATA;
						_RF8 = 1;

						DmaChnClrEvFlags(DMA_CHANNEL1, DMA_EV_BLOCK_DONE);
					} else {
						FSMInfo.nextState = DAQ_FATAL_ERROR;
					}

					break;

				case DAQ_SEND_BUFFER_B:
					if (DmaChnGetEvFlags(DMA_CHANNEL1) & DMA_EV_BLOCK_DONE) {
						BufferToSpi_TransferB(BufferB.index);
						BufferB.index = 0;
						FSMInfo.currentBuffer = BUFFER_A;
						FSMInfo.nextState = DAQ_GET_DATA;

						DmaChnClrEvFlags(DMA_CHANNEL1, DMA_EV_BLOCK_DONE);
					} else {
						FSMInfo.nextState = DAQ_FATAL_ERROR;
					}

					break;

				case DAQ_WAIT_FOR_CONVERSION:
		//			if (!BUSY_PORT) {
		//				if (FSMInfo.currentBuffer == BUFFER_A) {
		//					FSMInfo.nextState = DAQ_SAMPLES_TO_BUFFER_A;
		//				} else if (FSMInfo.currentBuffer == BUFFER_B) {
		//					FSMInfo.nextState = DAQ_SAMPLES_TO_BUFFER_B;
		//				} else {
		//					FSMInfo.nextState = DAQ_FATAL_ERROR;
		//				}
		//			} else {
		//				FSMInfo.nextState = DAQ_WAIT_FOR_CONVERSION;
		//			}
					break;

				case DAQ_WAIT:
					break;

				case DAQ_FATAL_ERROR:
					while (1) {
						LATBbits.LATB6 = 0;
						Nop();
					}//Until something better this will have to suffice.
					break;

				default:
					FSMInfo.nextState = DAQ_FATAL_ERROR;
					break;
				}
			}
		 */
	}
}

void InitFSM(void)
{
	FSMInfo.currentBuffer = BUFFER_A;
	FSMInfo.nextState = DAQ_INIT;
	FSMInfo.generalFlag = 0;
}