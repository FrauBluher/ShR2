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
 * @file	ADCModuleBoard.h
 * @author 	Pavlo Milo Manovi
 * @date	January, 2015
 * @brief 	This library is used to set up the peripherals of the pic and glue
 *              together all of the drivers in the library.  Pin pound defines can
 *              be found here.
 */

#ifndef ADCMODULEBOARD_H
#define	ADCMODULEBOARD_H

#include <xc.h>
#include <stdint.h>
#include "DMA_Transfer.h"
#include "MCP391x.h"

/**
 * @brief Pound defines for databit latch registers.
 */
#define DB0_LAT     LATEbits.LATE0
#define DB1_LAT     LATEbits.LATE1
#define DB2_LAT     LATEbits.LATE2
#define DB3_LAT     LATEbits.LATE3
#define DB4_LAT     LATEbits.LATE4
#define DB5_LAT     LATEbits.LATE5
#define DB6_LAT     LATEbits.LATE6
#define DB7_LAT     LATEbits.LATE7

/**
 * @brief Pound defines for databit tri-state registers.
 */
#define DB0_TRIS     TRISEbits.TRISE0
#define DB1_TRIS     TRISEbits.TRISE1
#define DB2_TRIS     TRISEbits.TRISE2
#define DB3_TRIS     TRISEbits.TRISE3
#define DB4_TRIS     TRISEbits.TRISE4
#define DB5_TRIS     TRISEbits.TRISE5
#define DB6_TRIS     TRISEbits.TRISE6
#define DB7_TRIS     TRISEbits.TRISE7

/**
 * @brief FIFO Control Lines - tristates.
 */
#define CS_TRIS TRISDbits.TRISD11
#define A0_TRIS TRISBbits.TRISB15
#define RD_TRIS TRISDbits.TRISD5
#define WR_TRIS TRISDbits.TRISD4

/**
 * @brief FIFO Control Lines - latches.
 */
#define CS_LAT LATDbits.LATD11
#define A0_LAT LATBbits.LATB15
#define RD_LAT LATDbits.LATD5
#define WR_LAT LATDbits.LATD4


/******************************************************************************/

/**
 * @brief SPI tristate registers and SS latch.
 */
#define SPI_SCK_TRIS    TRISDbits.TRISD10
#define SPI_MOSI_TRIS   TRISDbits.TRISD0
#define SPI_MISO_TRIS   TRISCbits.TRISC4
#define SPI_SS_TRIS     TRISCbits.TRISC3
#define SPI_SS_LAT      LATCbits.LATC3

/******************************************************************************/

/**
 * @brief MCP3912 control lines - tristates.
 */
#define DR_TRIS  TRISFbits.TRISF4 //Connected to pin sharing U3RX
#define RESET_TRIS TRISFbits.TRISF5 //Connected to pin sharing U3TX

/**
 * @brief MCP3912 control lines - latches.
 */
#define DR_LAT LATFbits.LATF4
#define RESET_LAT LATFbits.LATF5

/******************************************************************************/


/**
 * @brief UART Pins for debugging on Max32
 */
#define UART_TX_TRIS    TRISFbits.TRISF8
#define UART_RX_TRIS    TRISFbits.TRISF2

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/**
 * @brief Sets up the board with the peripherals defined in the header.
 */
uint8_t ADCModuleBoard_Init(SampleBuffer *BufferA, SampleBuffer *BufferB, MCP391x_Info *MCPInfo);

#endif	/* ADCMODULEBOARD_H */

