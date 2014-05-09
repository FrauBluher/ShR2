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

#include "ParallelIO.h"
#include "ADCModuleBoard.h"


/**
 * @file	ParallelIO.c
 * @author 	Pavlo Milo Manovi
 * @date	April, 2014
 * @brief 	This library provides the implementation of methods used to read
 *              and write to a parallel interface set up with individual bit
 *              registers.
 */

static PARALLEL_PORT_READ readReg;
static PARALLEL_PORT_WRITE writeReg;
static PARALLEL_PORT_TRISTATE tristateReg;

void Parallel_IO_Init(PARALLEL_PORT_WRITE latRegisters, PARALLEL_PORT_READ portRegisters, PARALLEL_PORT_TRISTATE trisRegisters)
{
	readReg = portRegisters;
	writeReg = latRegisters;
	tristateReg = trisRegisters;

	writeReg.wholeRegister = 0;
	tristateReg.wholeRegister = 0;
}

void Parallel_IO_Write(uint16_t parallelRegister)
{
	tristateReg.wholeRegister = 0;
	writeReg.wholeRegister = parallelRegister;
	tristateReg.wholeRegister = 0xFFFF;
}

uint16_t Parallel_IO_Read(void)
{
	uint16_t returnVal = 0;
	tristateReg.wholeRegister = 0xFFFF;
	returnVal = ((returnVal | (DB0_PORT)) | (returnVal | (DB1_PORT << 1)) |
		(returnVal | (DB2_PORT << 2)) | (returnVal | (DB3_PORT << 3)) |
		(returnVal | (DB4_PORT << 4)) | (returnVal | (DB5_PORT << 5)) |
		(returnVal | (DB6_PORT << 6)) | (returnVal | (DB7_PORT << 7)) |
		(returnVal | (DB8_PORT << 8)) | (returnVal | (DB9_PORT << 9)) |
		(returnVal | (DB10_PORT << 10)) | (returnVal | (DB11_PORT << 11)) |
		(returnVal | (DB12_PORT << 12)) | (returnVal | (DB13_PORT << 13)) |
		(returnVal | (DB14_PORT << 14)) | (returnVal | (DB15_PORT << 15)));
	return(returnVal);
}

