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
 * @file	ADS85x8.h
 * @author 	Pavlo Milo Manovi
 * @date	April, 2014
 * @brief 	This library is used to set up the peripherals of the pic and glue
 *              together all of the drivers in the library.
 */

#ifndef ADCMODULEBOARD_H
#define	ADCMODULEBOARD_H

//#define MAX32

#include <xc.h>
#include <stdint.h>
#include "DMA_Transfer.h"
#include "ADS85x8.h"

/**
 * @brief Pound defines for databit latch registers.
 */
#define DB0_LAT     LATDbits.LATD13
#define DB1_LAT     LATDbits.LATD5
#define DB2_LAT     LATFbits.LATF0
#define DB3_LAT     LATFbits.LATF1
#define DB4_LAT     LATGbits.LATG1 
#define DB5_LAT     LATGbits.LATG0 
#define DB6_LAT     LATAbits.LATA6
#define DB7_LAT     LATAbits.LATA7
#define DB8_LAT     LATEbits.LATE0
#define DB9_LAT     LATEbits.LATE1 
#define DB10_LAT    LATGbits.LATG14
#define DB11_LAT    LATGbits.LATG12
#define DB12_LAT    LATGbits.LATG13
#define DB13_LAT    LATEbits.LATE2
#define DB14_LAT    LATEbits.LATE3
#define DB15_LAT    LATEbits.LATE4

/**
 * @brief Pound defines for databit tri-state registers.
 */
#define DB0_TRIS     TRISDbits.TRISD13
#define DB1_TRIS     TRISDbits.TRISD5
#define DB2_TRIS     TRISFbits.TRISF0
#define DB3_TRIS     TRISFbits.TRISF1
#define DB4_TRIS     TRISGbits.TRISG1 
#define DB5_TRIS     TRISGbits.TRISG0 
#define DB6_TRIS     TRISAbits.TRISA6
#define DB7_TRIS     TRISAbits.TRISA7
#define DB8_TRIS     TRISEbits.TRISE0
#define DB9_TRIS     TRISEbits.TRISE1 
#define DB10_TRIS    TRISGbits.TRISG14
#define DB11_TRIS    TRISGbits.TRISG12
#define DB12_TRIS    TRISGbits.TRISG13
#define DB13_TRIS    TRISEbits.TRISE2
#define DB14_TRIS    TRISEbits.TRISE3
#define DB15_TRIS    TRISEbits.TRISE4

/**
 * @brief Pound defines for databit port registers.
 */
#define DB0_PORT     PORTDbits.RD13 
#define DB1_PORT     PORTDbits.RD5 
#define DB2_PORT     PORTFbits.RF0 
#define DB3_PORT     PORTFbits.RF1 
#define DB4_PORT     PORTGbits.RG1 
#define DB5_PORT     PORTGbits.RG0 
#define DB6_PORT     PORTAbits.RA6 
#define DB7_PORT     PORTAbits.RA7 
#define DB8_PORT     PORTEbits.RE0 
#define DB9_PORT     PORTEbits.RE1 
#define DB10_PORT    PORTGbits.RG14
#define DB11_PORT    PORTGbits.RG12
#define DB12_PORT    PORTGbits.RG13
#define DB13_PORT    PORTEbits.RE2 
#define DB14_PORT    PORTEbits.RE3 
#define DB15_PORT    PORTEbits.RE4 

/**
 * @brief SPI tristate registers and SS latch.
 */
#define SPI_SCK_TRIS    TRISDbits.TRISD10
#define SPI_MOSI_TRIS   TRISDbits.TRISD0
#define SPI_MISO_TRIS   TRISCbits.TRISC4
#define SPI_SS_TRIS     TRISCbits.TRISC3
#define SPI_SS_LAT      LATCbits.LATC3

/**
 * @brief ADS85x8 control lines - tristates.
 */
#define BUSY_TRIS   TRISDbits.TRISD14
#define CS_TRIS     TRISFbits.TRISF12
#define RD_TRIS     TRISBbits.TRISB12
#define WR_TRIS     TRISBbits.TRISB13
#define CONV_A_TRIS TRISBbits.TRISB15
#define CONV_B_TRIS TRISBbits.TRISB14
/*
 * #define CONV_C_TRIS 
 * #define CONV_D_TRIS
 * Define these two when you actually start using them.
 */

/**
 * @brief ADS85x8 control lines - latches.
 */
#define BUSY_PORT  PORTDbits.RD14
#define CS_LAT     LATFbits.LATF12
#define RD_LAT     LATBbits.LATB12
#define WR_LAT     LATBbits.LATB13
#define CONV_A_LAT LATBbits.LATB15
#define CONV_B_LAT LATBbits.LATB14
/*
 * #define CONV_C_LAT 
 * #define CONV_D_LAT
 * Define these two when you actually start using them.
 */

/**
 * @brief UART Pins for debugging on Max32
 */
#define UART_TX_TRIS    TRISGbits.TRISG8
#define UART_RX_TRIS    TRISGbits.TRISG7

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/**
 * @brief Sets up the board with the peripherals defined in the header.
 */
uint8_t ADCModuleBoard_Init(SampleBuffer *BufferA, SampleBuffer *BufferB, ADS85x8_Info *DS85x8Info);

#endif	/* ADCMODULEBOARD_H */

