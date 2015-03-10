#define SUPPRESS_PLIB_WARNING
#include "ADCModuleBoard.h"
#include <stdio.h>
#include <plib.h>

SampleBuffer BufferA;
SampleBuffer BufferB;

typedef struct {
    uint8_t currentBuffer;
    uint8_t nextState;
    uint8_t generalFlag;
} DAQFSMInfo;

static ADS85x8_Info ADCInfo;
static DAQFSMInfo FSMInfo;

int main(void) {
    ADCModuleBoard_Init(&BufferA, &BufferB, &ADCInfo);
    _TRISB6 = 0;
    _RB6 = 1;

    while (1) {
//        if (DmaChnGetEvFlags(DMA_CHANNEL1) & DMA_EV_BLOCK_DONE) {
//            BufferToUART3_TransferA(BufferA.index);
//            DmaChnClrEvFlags(DMA_CHANNEL1, DMA_EV_BLOCK_DONE);
//        }
    }
}