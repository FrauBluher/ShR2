#include "ADCModuleBoard.h"
#include <plib.h>

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

	InitFSM();

	//Some delay for the ADC to power up.  This can be fixed a little later.
	for (temp = 0; temp < 10000; temp++) {
		Nop();
	}

	while (1) {
		if (PORTFbits.RF2) {
			//For debugging the effective sample rate can be lowered by putting a delay before
			//the switch statement, i.e. here.
			for (temp = 0; temp < 50; temp++) {
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
				_RF8 = 0;
				CONV_A_LAT = 0;
				CONV_B_LAT = 0;
				break;

			case DAQ_SAMPLES_TO_BUFFER_A:
				if (BufferA.index < BUFFERLENGTH - 1) {
					ADS85x8_GetSamples();
					if (ADCInfo.newData) {
						BufferA.BufferArray[BufferA.index] = (ADCInfo.sampledDataChA0 & 0xFF00) >> 8;
						BufferA.index++;
						BufferA.BufferArray[BufferA.index] = (ADCInfo.sampledDataChA0 & 0x00FF);
						BufferA.index++;

						FSMInfo.nextState = DAQ_START_CONVERSION;
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

						FSMInfo.nextState = DAQ_START_CONVERSION;
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
					FSMInfo.nextState = DAQ_START_CONVERSION;
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
					FSMInfo.nextState = DAQ_START_CONVERSION;
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
}

void InitFSM(void)
{
	FSMInfo.currentBuffer = BUFFER_A;
	FSMInfo.nextState = DAQ_INIT;
	FSMInfo.generalFlag = 0;
}