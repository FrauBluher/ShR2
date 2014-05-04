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
 * @brief 	This library is used to set up the peripherals of the pic.
 */

#ifndef ADCMODULEBOARD_H
#define	ADCMODULEBOARD_H

#define MAX32

#include <xc.h>
#include <stdint.h>
#include "SPI_DMA_Transfer.h"

#ifndef MAX32 //Used for the ADC Module

/**
 * @brief Pound defines for databit latch registers.
 */
#define DB0_LAT     LATDbits.LATD13
#define DB1_LAT     LATDbits.LATD5
#define DB2_LAT     LATFbits.LATF0
#define DB3_LAT     LATFbits.LATF1
#define DB4_LAT     LATGbits.LATG0
#define DB5_LAT     LATGbits.LATG1
#define DB6_LAT     LATAbits.LATA6
#define DB7_LAT     LATAbits.LATA7
#define DB8_LAT     LATEbits.LATE0
#define DB9_LAT     LATEbits.LATE9
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
#define DB4_TRIS     TRISGbits.TRISG0
#define DB5_TRIS     TRISGbits.TRISG1
#define DB6_TRIS     TRISAbits.TRISA6
#define DB7_TRIS     TRISAbits.TRISA7
#define DB8_TRIS     TRISEbits.TRISE0
#define DB9_TRIS     TRISEbits.TRISE9
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
#define DB4_PORT     PORTGbits.RG0
#define DB5_PORT     PORTGbits.RG1
#define DB6_PORT     PORTAbits.RA6
#define DB7_PORT     PORTAbits.RA7
#define DB8_PORT     PORTEbits.RE0
#define DB9_PORT     PORTEbits.RE9
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
 * UNUSED FOR THE ADC MODULE TESTER BOARD AS OF NOW 
 */

/**
 * @brief ADS85x8 control lines - latches.
 */
#define BUSY_LAT   LATDbits.LATD14
#define CS_LAT     LATFbits.LATF12
#define RD_LAT     LATBbits.LATB12
#define WR_LAT     LATBbits.LATB13
#define CONV_A_LAT LATBbits.LATB15
#define CONV_B_LAT LATBbits.LATB14
/*
 * #define CONV_C_LAT 
 * #define CONV_D_LAT
 * UNUSED FOR THE ADC MODULE TESTER BOARD AS OF NOW 
 */

#endif

#ifdef MAX32 //Used for the tester board implemented with the Max32.
/**
 * @brief Pound defines for databit latch registers.
 */
#define DB0_LAT     LATEbits.LATE9 // 7
#define DB1_LAT     LATDbits.LATD2 // 6
#define DB2_LAT     LATDbits.LATD1 // 5
#define DB3_LAT     LATGbits.LATG2 //27
#define DB4_LAT     LATGbits.LATG7 //29
#define DB5_LAT     LATEbits.LATE6 //31
#define DB6_LAT     LATEbits.LATE4 //33
#define DB7_LAT     LATFbits.LATF3 //25
#define DB8_LAT     LATEbits.LATE3 //34
#define DB9_LAT     LATEbits.LATE5 //32
#define DB10_LAT    LATEbits.LATE7 //30
#define DB11_LAT    LATGbits.LATG15 //28
#define DB12_LAT    LATGbits.LATG3 //26
#define DB13_LAT    LATCbits.LATC13 // 75
#define DB14_LAT    LATDbits.LATD13 // 76
#define DB15_LAT    LATDbits.LATD7 // 77
/**
 * @brief Pound defines for databit tri-state registers.
 */
#define DB0_TRIS     TRISEbits.TRISE9 // 7
#define DB1_TRIS     TRISDbits.TRISD2 // 6
#define DB2_TRIS     TRISDbits.TRISD1 // 5
#define DB3_TRIS     TRISGbits.TRISG2 //27
#define DB4_TRIS     TRISGbits.TRISG7 //29
#define DB5_TRIS     TRISEbits.TRISE6 //31
#define DB6_TRIS     TRISEbits.TRISE4 //33
#define DB7_TRIS     TRISFbits.TRISF3 //25
#define DB8_TRIS     TRISEbits.TRISE3 //34
#define DB9_TRIS     TRISEbits.TRISE5 //32
#define DB10_TRIS    TRISEbits.TRISE7 //30
#define DB11_TRIS    TRISGbits.TRISG15 //28
#define DB12_TRIS    TRISGbits.TRISG3 //26
#define DB13_TRIS    TRISCbits.TRISC13 // 75
#define DB14_TRIS    TRISDbits.TRISD13 // 76
#define DB15_TRIS    TRISDbits.TRISD7 // 77

/**
 * @brief Pound defines for databit port registers.
 */
#define DB0_PORT     PORTEbits.RE9 // 7
#define DB1_PORT     PORTDbits.RD2 // 6
#define DB2_PORT     PORTDbits.RD1 // 5
#define DB3_PORT     PORTGbits.RG2 //27
#define DB4_PORT     PORTGbits.RG7 //29
#define DB5_PORT     PORTEbits.RE6 //31
#define DB6_PORT     PORTEbits.RE4 //33
#define DB7_PORT     PORTFbits.RF3 //25
#define DB8_PORT     PORTEbits.RE3 //34
#define DB9_PORT     PORTEbits.RE5 //32
#define DB10_PORT    PORTEbits.RE7 //30
#define DB11_PORT    PORTGbits.RG15 //28
#define DB12_PORT    PORTGbits.RG3 //26
#define DB13_PORT    PORTCbits.RC13 // 75
#define DB14_PORT    PORTDbits.RD13 // 76
#define DB15_PORT    PORTDbits.RD7 // 77

/**
 * @brief SPI tristate registers and SS latch.
 */
#define SPI_SCK_TRIS    TRISDbits.TRISD10 //38 SCK1
#define SPI_MOSI_TRIS   TRISDbits.TRISD0  //3 SDO1
#define SPI_MISO_TRIS   TRISCbits.TRISC4  //11 SDI1
#define SPI_SS_TRIS     TRISAbits.TRISA3  //13 SS
#define SPI_SS_LAT      LATAbits.LATA3    //13

/**
 * @brief ADS85x8 control lines - tristates.
 */
#define BUSY_TRIS   TRISDbits.TRISD2 //8
#define CS_TRIS     TRISAbits.TRISA6 //80
#define RD_TRIS     TRISGbits.TRISG0 //79
#define WR_TRIS     TRISGbits.TRISG1 //78
#define CONV_A_TRIS TRISDbits.TRISD3 //9
#define CONV_B_TRIS TRISDbits.TRISD4 //10
/*
 * #define CONV_C_TRIS
 * #define CONV_D_TRIS
 * UNUSED FOR THE ADC MODULE TESTER BOARD AS OF NOW
 */

/**
 * @brief ADS85x8 control lines - latches.
 * BUSY is read only so it's a PORT reg.
 */
#define BUSY_PORT  PORTDbits.RD2 //8
#define CS_LAT     LATAbits.LATA6 //80
#define RD_LAT     LATGbits.LATG0 //79
#define WR_LAT     LATGbits.LATG1 //78
#define CONV_A_LAT LATDbits.LATD3 //9
#define CONV_B_LAT LATDbits.LATD4 //10
/*
 * #define CONV_C_LAT
 * #define CONV_D_LAT
 * UNUSED FOR THE ADC MODULE TESTER BOARD AS OF NOW
 */

/**
 * @brief UART Pins for debugging on Max32
 */
#define UART_TX_TRIS    TRISFbits.TRISF8
#define UART_RX_TRIS    TRISFbits.TRISF2
#endif

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/**
 * @brief Sets up the board with the peripherals defined in the header.
 */
uint8_t ADCModuleBoard_Init(SampleBuffer *BufferA, SampleBuffer *BufferB);

#endif	/* ADCMODULEBOARD_H */

