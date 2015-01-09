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
#include <plib.h>
#include "DMA_Transfer.h"
#include "MCP391x.h"

#if defined (__32MX360F512L__) || (__32MX460F512L__) || (__32MX795F512L__) || (__32MX430F064L__) || (__32MX450F256L__) || (__32MX470F512L__) || (__32MX320F128L__)
// Configuration Bit settings
// SYSCLK = 80 MHz (8MHz Crystal / FPLLIDIV * FPLLMUL / FPLLODIV)
// PBCLK = 80 MHz (SYSCLK / FPBDIV)
// Primary Osc w/PLL (XT+,HS+,EC+PLL)
// WDT OFF
// Other options are don't care


//TODO: SET CONFIG FUSE BITS FOR EXTERNAL OSCILLATOR.
#pragma config FNOSC = FRC // Oscillator Selection Bits (Fast RC Osc //(FRC))
#pragma config FSOSCEN = ON // Secondary Oscillator Enable (Enabled)
#pragma config IESO = OFF // Internal/External Switch Over (Disabled)
#pragma config POSCMOD = OFF // Primary Oscillator Configuration (Primary //osc disabled)

#pragma config FPLLIDIV = DIV_2
#pragma config FPLLMUL = MUL_20
#pragma config FPLLODIV = DIV_1

#pragma config OSCIOFNC = OFF // CLKO Output Signal Active on the OSCO Pin //(Disabled)
#pragma config FPBDIV = DIV_1 // Peripheral Clock Divisor (Pb_Clk is //Sys_Clk/8)
#pragma config FCKSM = CSDCMD // Clock Switching and Monitor Selection//(Clock Switch Disable, FSCM Disabled)
#pragma config FWDTEN = OFF // Watchdog Timer Enable (WDT Disabled (SWDTEN //Bit Controls))
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
uint8_t ADCModuleBoard_Init(SampleBuffer *BufferA, SampleBuffer *BufferB, MCP391x_Info *MCPInfo)
{
	SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

	LATA = 0;
	LATB = 0;
	LATC = 0;
	LATD = 0;
	LATE = 0;
	LATF = 0;
	LATG = 0;
	AD1PCFG = 0xFFFFFFFF;

	//Set up tristates for the dataports and R/W stuff for the FIFO interface

	//SPI Tristate setup
	SPI_MISO_TRIS = 1;
	SPI_MOSI_TRIS = 0;
	SPI_SCK_TRIS = 0;
	SPI_SS_TRIS = 0;

	//UART tristate setup
	UART_TX_TRIS = 0;
	UART_RX_TRIS = 1;

	//MCP tristate setup
	DR_TRIS = 1;
	RESET_TRIS = 0;

	//FIFO tristate setup
	DB0_TRIS = 0;
	DB1_TRIS = 0;
	DB2_TRIS = 0;
	DB3_TRIS = 0;
	DB4_TRIS = 0;
	DB5_TRIS = 0;
	DB6_TRIS = 0;
	DB7_TRIS = 0;

	CS_TRIS = 0;
	A0_TRIS = 0;
	RD_TRIS = 0;
	WR_LAT = 0;

	/*
	 * Depending on if you want to use a DMA transfer and what peripheral you
	 * want to transmit data on you use the following:
	 * - BufferToUART_Init(BufferA, BufferB);
	 * - BufferToSpi_Init(BufferA, BufferB)
	 */

	RESET_LAT = 1;
	SPI_SS_LAT = 1;

	//Weak pull-downs for DR and MISO
	CNPUE;

	BufferToSpi_Init(BufferA, BufferB);
	MCP391x_Init(MCPInfo);

	INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
	INTEnableInterrupts();

	//System Initialized Successfully
	return(EXIT_SUCCESS);
}
