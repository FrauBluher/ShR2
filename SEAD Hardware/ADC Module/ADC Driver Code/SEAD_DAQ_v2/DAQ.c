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

static ADS85x8_Info ADCInfo;
static DAQFSMInfo FSMInfo;

int main(void)
{
	int16_t temp;

	InitFSM();
	while (1) {
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
			//This state may need to be renamed, but this is where
			FSMInfo.nextState = DAQ_WAIT_FOR_CONVERSION;
			break;

		case DAQ_SAMPLES_TO_BUFFER_A:
			if (BufferA.index < BUFFERLENGTH - 1) {
				ADS85x8_GetSamples();
				if (ADCInfo.newData) {
					BufferA.BufferArray[BufferA.index] = (ADCInfo.sampledDataChA0 & 0xFF00) >> 8;
					BufferA.index++;
					BufferA.BufferArray[BufferA.index] = (ADCInfo.sampledDataChA0 & 0x00FF);
					BufferA.index++;

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
				ADS85x8_GetSamples();
				if (ADCInfo.newData) {

					BufferB.BufferArray[BufferB.index] = (ADCInfo.sampledDataChA0 & 0xFF00) >> 8;
					BufferB.index++;
					BufferB.BufferArray[BufferB.index] = (ADCInfo.sampledDataChA0 & 0x00FF);
					BufferB.index++;

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
				_RF8 = 1;

				DmaChnClrEvFlags(DMA_CHANNEL1, DMA_EV_BLOCK_DONE);
			} else {
				FSMInfo.nextState = DAQ_FATAL_ERROR;
			}

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
				LATBbits.LATB6 = 0;
				Nop();
			}//Until something better this will have to suffice.
			break;

		default:
			FSMInfo.nextState = DAQ_FATAL_ERROR;
			break;
		}
	} else {
		Nop();
	}
}

void InitFSM(void)
{
	FSMInfo.currentBuffer = BUFFER_A;
	FSMInfo.nextState = DAQ_INIT;
	FSMInfo.generalFlag = 0;
}