/*********************************************************************
 *
 *     DMA Uart Echo Example
 *
 *********************************************************************
 * FileName:        uart_echo.c
 * Dependencies:    plib.h
 *
 * Processor:       PIC32MX
 *
 * Complier:        MPLAB C32 v1 or higher
 *                  MPLAB IDE v8 or higher
 * Company:         Microchip Technology Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the ?Company?) for its PIC Microcontroller is intended
 * and supplied to you, the Company?s customer, for use solely and
 * exclusively on Microchip PIC Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN ?AS IS? CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *********************************************************************
 * $Id: dma_api_example.c 4261 2007-08-22 16:32:28Z aura $
 *********************************************************************
 * Platform: Explorer-16 with PIC32MX PIM
 *
 * Description:
 *      Receives data from the UART module and echoes it back using
 *      the DMA Peripheral Lib. The received data is expected to end
 *      with a CR and has to be less than DmaGetMaxTxferSize() bytes
 *      in length. We'll enable the DMA interrupts to signal to us
 *      when the transfer is done.
 ********************************************************************/
#include <plib.h>
#include "ADCModuleBoard.h"
#include "ParallelIO.h"
#include "ADS85x8.h"

SampleBuffer BufferA;
SampleBuffer BufferB;

static uint8_t flag = 0;

void InitFSM(void);

enum {
	DAQ_INIT,
	DAQ_START_CONVERSION,
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

static ADS85x8_Info ADCInfo;
static DAQFSMInfo FSMInfo;

int main(void)
{
	int16_t temp;
	uint8_t i;

	InitFSM();

	//Some delay for the ADC to power up.  This can be fixed a little later.
	for (temp = 0; temp < 10000; temp++) {
		Nop();
	}

	while (1) {
		//For debugging the effective sample rate can be lowered by putting a delay before
		//the switch statement, i.e. here.
		for (temp = 0; temp < 10000; temp++) {
			Nop();
			//LATAbits.LATA1 = 1;
		}


		//TODO: Put bit toggling into functions somewhere else as to not clutter up the FSM.
		switch (FSMInfo.nextState) {
		case DAQ_INIT:
			if (ADCModuleBoard_Init(&BufferA, &BufferB, &ADCInfo) == EXIT_SUCCESS) {
				flag = 1;
				FSMInfo.nextState = DAQ_START_CONVERSION;
			} else {
				FSMInfo.nextState = DAQ_FATAL_ERROR;
			}
			break;

		case DAQ_START_CONVERSION:
			/*
			 * The CONV_X High to BUSY High delay is maximally 25ns, which means
			 * that we need to wait at least 25ns before using it in a state transition
			 * case. Each Nop() should provide 12.5nS of delay
			 */
			RD_LAT = 1; //Ensure RD is high.
			CS_LAT = 1; //Ensure CS is high.
			CONV_A_LAT = 1;
			CONV_B_LAT = 1;
			//Nop();
			FSMInfo.nextState = DAQ_WAIT_FOR_CONVERSION;
			while (!BUSY_PORT); // Until I figure out tight timings we wait for BUSY to go high for testing.
			CONV_A_LAT = 0;
			CONV_B_LAT = 0;
			break;

		case DAQ_SAMPLES_TO_BUFFER_A:
			//For UART we split the 16 bit long value into two bytes...
			if (BufferA.index < BUFFERLENGTH - 8) {
				ADS85x8_GetSamples();
				if (ADCInfo.newData) {
					BufferA.BufferArray[BufferA.index] = (ADCInfo.sampledDataChA0 & 0xFF00) >> 8;
					BufferA.index++;
					BufferA.BufferArray[BufferA.index] = (ADCInfo.sampledDataChA0 & 0x00FF);
					BufferA.index++;

					BufferA.BufferArray[BufferA.index] = (ADCInfo.sampledDataChA1 & 0xFF00) >> 8;
					BufferA.index++;
					BufferA.BufferArray[BufferA.index] = (ADCInfo.sampledDataChA1 & 0x00FF);
					BufferA.index++;

					BufferA.BufferArray[BufferA.index] = (ADCInfo.sampledDataChB1 & 0xFF00) >> 8;
					BufferA.index++;
					BufferA.BufferArray[BufferA.index] = (ADCInfo.sampledDataChB1 & 0x00FF);
					BufferA.index++;

					FSMInfo.nextState = DAQ_START_CONVERSION;
				} else {
					FSMInfo.nextState = DAQ_SEND_BUFFER_A;
				}
			} else {
				FSMInfo.nextState = DAQ_FATAL_ERROR;
			}
			break;

		case DAQ_SAMPLES_TO_BUFFER_B:
			//For UART we split the 16 bit long value into two bytes...
			if (BufferB.index < BUFFERLENGTH - 8) {
				ADS85x8_GetSamples();
				if (ADCInfo.newData) {
					BufferB.BufferArray[BufferB.index] = (ADCInfo.sampledDataChA0 & 0xFF00) >> 8;
					BufferB.index++;
					BufferB.BufferArray[BufferB.index] = (ADCInfo.sampledDataChA0 & 0x00FF);
					BufferB.index++;

					BufferB.BufferArray[BufferB.index] = (ADCInfo.sampledDataChA1 & 0xFF00) >> 8;
					BufferB.index++;
					BufferB.BufferArray[BufferB.index] = (ADCInfo.sampledDataChA1 & 0x00FF);
					BufferB.index++;

					BufferB.BufferArray[BufferB.index] = (ADCInfo.sampledDataChB1 & 0xFF00) >> 8;
					BufferB.index++;
					BufferB.BufferArray[BufferB.index] = (ADCInfo.sampledDataChB1 & 0x00FF);
					BufferB.index++;

					FSMInfo.nextState = DAQ_START_CONVERSION;
				} else {
					FSMInfo.nextState = DAQ_SEND_BUFFER_B;
				}
			} else {
				FSMInfo.nextState = DAQ_FATAL_ERROR;
			}
			break;

		case DAQ_SEND_BUFFER_A:
			if (DmaChnGetEvFlags(DMA_CHANNEL1) & DMA_EV_BLOCK_DONE || flag) {
				flag = 0;
				BufferToUART_TransferA(BufferA.index);
				BufferA.index = 0;
				FSMInfo.currentBuffer = BUFFER_B;
				FSMInfo.nextState = DAQ_START_CONVERSION;

				DmaChnClrEvFlags(DMA_CHANNEL1, DMA_EV_BLOCK_DONE);
			} else {
				FSMInfo.nextState = DAQ_FATAL_ERROR;
				//FSMInfo.nextState = DAQ_START_CONVERSION;
			}

			break;

		case DAQ_SEND_BUFFER_B:
			if (DmaChnGetEvFlags(DMA_CHANNEL1) & DMA_EV_BLOCK_DONE) {
				BufferToUART_TransferB(BufferB.index);
				BufferB.index = 0;
				FSMInfo.currentBuffer = BUFFER_A;
				FSMInfo.nextState = DAQ_START_CONVERSION;

				DmaChnClrEvFlags(DMA_CHANNEL1, DMA_EV_BLOCK_DONE);
			} else {
				FSMInfo.nextState = DAQ_FATAL_ERROR;
				//FSMInfo.nextState = DAQ_START_CONVERSION;
			}

			break;

			break;

		case DAQ_WAIT_FOR_CONVERSION:
			if (!BUSY_PORT) {
				if (FSMInfo.currentBuffer == BUFFER_A) {
					FSMInfo.nextState = DAQ_SAMPLES_TO_BUFFER_A;
				} else if (FSMInfo.currentBuffer == BUFFER_B) {
					FSMInfo.nextState = DAQ_SAMPLES_TO_BUFFER_B;
				} else {
					FSMInfo.nextState = DAQ_FATAL_ERROR;
				}
			} else {
				FSMInfo.nextState = DAQ_WAIT_FOR_CONVERSION;
			}
			break;
		case DAQ_WAIT:
			break;

		case DAQ_FATAL_ERROR:
			/*
			 * Turn on an LED or something here.  The code should enter this
			 * part of the FSM on buffer overflows (I.E. Buffer A fills up while
			 * buffer B is still transmitting.  Crash and burn.
			 */
			while (1) {
				Nop();
			}//Until something better this will have to suffice.
			break;

		default:
			FSMInfo.nextState = DAQ_FATAL_ERROR;
			break;
		}
	}
}

void InitFSM(void)
{
	FSMInfo.currentBuffer = BUFFER_A;
	FSMInfo.nextState = DAQ_INIT;
	FSMInfo.generalFlag = 0;
}
// Configure the Timer 1 interrupt handler

//void __ISR(_TIMER_1_VECTOR, ipl2) Timer1Handler(void)
//{
//	if (!flag) {
//		putsUART1("\r\n");
//		BufferToUART_TransferA(10);
//		flag = 1;
//	} else {
//		putsUART1("\r\n");
//		BufferToUART_TransferB(10);
//		flag = 0;
//	}
//	// Clear the interrupt flag
//	INTClearFlag(INT_T1);
//
//	// Toggle LEDs on the Explorer-16
//	mPORTAToggleBits(BIT_3 | BIT_2 | BIT_1 | BIT_0);
//}