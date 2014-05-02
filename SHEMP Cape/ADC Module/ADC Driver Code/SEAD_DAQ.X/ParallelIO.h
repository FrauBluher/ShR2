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
 * @file	ParallelIO.h
 * @author 	Pavlo Milo Manovi
 * @date	April, 2014
 * @brief 	This library provides datatypes to convert individual bits into 16 bit words.
 */

#ifndef PARALLELIO_H
#define	PARALLELIO_H

#include <stdint.h>

/**
 * @brief Used to map different LAT registers to one 16 bit word for writing.
 */
typedef struct {

    union {
        uint16_t wholeRegister;

        struct {
            unsigned BIT0 : 1;
            unsigned BIT1 : 1;
            unsigned BIT2 : 1;
            unsigned BIT3 : 1;
            unsigned BIT4 : 1;
            unsigned BIT5 : 1;
            unsigned BIT6 : 1;
            unsigned BIT7 : 1;
            unsigned BIT8 : 1;
            unsigned BIT9 : 1;
            unsigned BIT10 : 1;
            unsigned BIT11 : 1;
            unsigned BIT12 : 1;
            unsigned BIT13 : 1;
            unsigned BIT14 : 1;
            unsigned BIT15 : 1;
        };
    };
} PARALLEL_PORT_WRITE;

/**
 * @brief Used to map different PORT registers to one 16 bit word for reading.
 */
typedef struct {

    union {
        uint16_t wholeRegister;

        struct {
            unsigned BIT0 : 1;
            unsigned BIT1 : 1;
            unsigned BIT2 : 1;
            unsigned BIT3 : 1;
            unsigned BIT4 : 1;
            unsigned BIT5 : 1;
            unsigned BIT6 : 1;
            unsigned BIT7 : 1;
            unsigned BIT8 : 1;
            unsigned BIT9 : 1;
            unsigned BIT10 : 1;
            unsigned BIT11 : 1;
            unsigned BIT12 : 1;
            unsigned BIT13 : 1;
            unsigned BIT14 : 1;
            unsigned BIT15 : 1;
        };
    };
} PARALLEL_PORT_READ;

/**
 * @brief Used to map different TRIS registers to one 16 bit word for tristate manipulation.
 */
typedef struct {

    union {
        uint16_t wholeRegister;

        struct {
            unsigned BIT0 : 1;
            unsigned BIT1 : 1;
            unsigned BIT2 : 1;
            unsigned BIT3 : 1;
            unsigned BIT4 : 1;
            unsigned BIT5 : 1;
            unsigned BIT6 : 1;
            unsigned BIT7 : 1;
            unsigned BIT8 : 1;
            unsigned BIT9 : 1;
            unsigned BIT10 : 1;
            unsigned BIT11 : 1;
            unsigned BIT12 : 1;
            unsigned BIT13 : 1;
            unsigned BIT14 : 1;
            unsigned BIT15 : 1;
        };
    };
} PARALLEL_PORT_TRISTATE;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void Parallel_IO_Init(PARALLEL_PORT_WRITE latRegisters, PARALLEL_PORT_READ portRegisters, PARALLEL_PORT_TRISTATE trisRegisters);

void Parallel_IO_Write(uint16_t parallelRegister);

uint16_t Parallel_IO_Read(void);
#endif	/* PARALLELIO_H */

