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

SampleBuffer BufferA;
SampleBuffer BufferB;

static uint8_t flag = 0;

int main(void)
{
	ADCModuleBoard_Init(&BufferA, &BufferB);

	int i;
	for (i = 0; i < 301; i++) {
		BufferA.BufferArray[i] = 'A';
		BufferB.BufferArray[i] = 'B';
	}

	BufferA.bufferFull = 1;

	while (1);

}


// Configure the Timer 1 interrupt handler

void __ISR(_TIMER_1_VECTOR, ipl2) Timer1Handler(void)
{
	if (!flag) {
		putsUART1("\r\n");
		BufferToUART_TransferA(10);
		flag = 1;
	} else {
		putsUART1("\r\n");
		BufferToUART_TransferB(10);
		flag = 0;
	}
	// Clear the interrupt flag
	INTClearFlag(INT_T1);

	// Toggle LEDs on the Explorer-16
	mPORTAToggleBits(BIT_3 | BIT_2 | BIT_1 | BIT_0);
}


