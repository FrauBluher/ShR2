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

enum {
	INPUT,
	OUTPUT
};

static PARALLEL_PORT_READ readReg;
static PARALLEL_PORT_WRITE writeReg;
static PARALLEL_PORT_TRISTATE tristateReg;

void SetTris(uint8_t io);

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
	SetTris(OUTPUT);

	writeReg.wholeRegister = parallelRegister;

	DB0_PORT = (writeReg.wholeRegister & 0x0001);
	DB1_PORT = (writeReg.wholeRegister & 0x0002) >> 1;
	DB2_PORT = (writeReg.wholeRegister & 0x0004) >> 2;
	DB3_PORT = (writeReg.wholeRegister & 0x0008) >> 3;
	DB4_PORT = (writeReg.wholeRegister & 0x0010) >> 4;
	DB5_PORT = (writeReg.wholeRegister & 0x0020) >> 5;
	DB6_PORT = (writeReg.wholeRegister & 0x0040) >> 6;
	DB7_PORT = (writeReg.wholeRegister & 0x0080) >> 7;
	DB8_PORT = (writeReg.wholeRegister & 0x0100) >> 8;
	DB9_PORT = (writeReg.wholeRegister & 0x0200) >> 9;
	DB10_PORT = (writeReg.wholeRegister & 0x0400) >> 10;
	DB11_PORT = (writeReg.wholeRegister & 0x0800) >> 11;
	DB12_PORT = (writeReg.wholeRegister & 0x1000) >> 12;
	DB13_PORT = (writeReg.wholeRegister & 0x2000) >> 13;
	DB14_PORT = (writeReg.wholeRegister & 0x4000) >> 14;
	DB15_PORT = (writeReg.wholeRegister & 0x8000) >> 15;
}

uint16_t Parallel_IO_Read(void)
{
	uint16_t returnVal = 0;
	SetTris(INPUT);

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

void SetTris(uint8_t io)
{
	if (io == INPUT) {
		DB0_TRIS = 1;
		DB1_TRIS = 1;
		DB2_TRIS = 1;
		DB3_TRIS = 1;
		DB4_TRIS = 1;
		DB5_TRIS = 1;
		DB6_TRIS = 1;
		DB7_TRIS = 1;
		DB8_TRIS = 1;
		DB9_TRIS = 1;
		DB10_TRIS = 1;
		DB11_TRIS = 1;
		DB12_TRIS = 1;
		DB13_TRIS = 1;
		DB14_TRIS = 1;
		DB15_TRIS = 1;
	} else {
		DB0_TRIS = 0;
		DB1_TRIS = 0;
		DB2_TRIS = 0;
		DB3_TRIS = 0;
		DB4_TRIS = 0;
		DB5_TRIS = 0;
		DB6_TRIS = 0;
		DB7_TRIS = 0;
		DB8_TRIS = 0;
		DB9_TRIS = 0;
		DB10_TRIS = 0;
		DB11_TRIS = 0;
		DB12_TRIS = 0;
		DB13_TRIS = 0;
		DB14_TRIS = 0;
		DB15_TRIS = 0;
	}
}
