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
 * @file	ADCModuleBoard.c
 * @author 	Pavlo Milo Manovi
 * @date	April, 2014
 * @brief 	Provides implementation to methods which set up the board for
 *		data acquisition.
 */

#include "ADCModuleBoard.h"
#include "ParallelIO.h"
#include <plib.h>
#include "DMA_Transfer.h"
#include "ADS85x8.h"

#if defined (__32MX360F512L__) || (__32MX460F512L__) || (__32MX795F512L__) || (__32MX430F064L__) || (__32MX450F256L__) || (__32MX470F512L__) || (__32MX320F128L__)
// Configuration Bit settings
// Configuration Bit settings
// SYSCLK = 80 MHz (8MHz Crystal / FPLLIDIV * FPLLMUL / FPLLODIV)
// PBCLK = 80 MHz (SYSCLK / FPBDIV)
// Primary Osc w/PLL (XT+,HS+,EC+PLL)
// WDT OFF
// Other options are don't care
#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_1
#define SYS_FREQ (80000000L)

#pragma config OSCIOFNC = OFF // CLKO Output Signal Active on the OSCO Pin //(Disabled)
#pragma config FCKSM = CSDCMD // Clock Switching and Monitor Selection//(Clock Switch Disable, FSCM Disabled)
#pragma config ICESEL = ICS_PGx1 // ICE/ICD Comm Channel Select (Communicate on //PGEC1/PGED1)
#pragma config PWP = OFF // Program Flash Write Protect (Disable)
#pragma config BWP = OFF // Boot Flash Write Protect bit (Protection //Disabled)
#pragma config CP = OFF // Code Protect (Protection Disabled)


#define SYS_FREQ (80000000L)
#define	GetPeripheralClock()		(SYS_FREQ/(1 << OSCCONbits.PBDIV))
#define UART_MODULE_ID 1
#endif

/**
 * @brief Sets up the board with the peripherals defined in the header.
 */
uint8_t ADCModuleBoard_Init(SampleBuffer *BufferA, SampleBuffer *BufferB, ADS85x8_Info *DS85x8Info)
{
	SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);
        mOSCSetPBDIV( OSC_PB_DIV_1 );

	AD1PCFG = 0xFFFFFFFF;

	//UART tristate setup
	UART_TX_TRIS = 0;
	UART_RX_TRIS = 1;

	/*
	 * Depending on if you want to use a DMA transfer and what peripheral you
	 * want to transmit data on you use the following:
	 * - BufferToUART_Init(BufferA, BufferB);
	 * - BufferToSpi_Init(BufferA, BufferB)
	 */

	//BufferToSpi_Init(BufferA, BufferB);
        BufferToUART3_Init(BufferA, BufferB);

	INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
	INTEnableInterrupts();

	//System Initialized Successfully
	return(EXIT_SUCCESS);
}
