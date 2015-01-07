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
 * @file	
 * @author 	Pavlo Milo Manovi
 * @date	April, 2014
 * @brief 	This library provides implementation of methods to init and read the MCP391x.
 *
 */

#include "MCP391x.h"
#include "ADCModuleBoard.h"
#include "ParallelIO.h"
#include <stdint.h>
#include <stdlib.h>

static MCP391x_Info *passedInfoStruct;

/**
 * @brief Sets up the MCP391x.
 * @param MCP391x_Params A pointer to a MCP391x_Info struct which will be updated.
 * @return Returns EXIT_SUCCESS if the device responds with the set configuration.
 */
uint8_t MCP391x_Init(MCP391x_Info *MCP391xInfo)
{
	MCP391x_Info MCP;

	//Setting bits for configuration of the MCP3912;
	MCP.config0Reg.BOOST = 0b11;
	MCP.config0Reg.DITHER = 0b11;
	MCP.config0Reg.EN_GAINCAL = 0;
	MCP.config0Reg.EN_OFFCAL = 0;
	MCP.config0Reg.OSR = 0b010;
	MCP.config0Reg.PRE = 0b10;
	MCP.config0Reg.VREFCAL = 64;

	MCP.config1Reg.CLKEXT = 1;
	MCP.config1Reg.RESET = 0b0000;
	MCP.config1Reg.SHUTDOWN = 0b0000;
	MCP.config1Reg.VREFEXT = 0;

	MCP.phaseReg.PHASEA = 0;
	MCP.phaseReg.PHASEB = 0;

	MCP.statusReg.WIDTH_DATA = 0b00;
	MCP.statusReg.DR_HIZ = 0;
	MCP.statusReg.DR_LINK = 1;
	MCP.statusReg.EN_CRCCOM = 0;
	MCP.statusReg.EN_INT = 0;
	MCP.statusReg.READ = 0b10;
	MCP.statusReg.WIDTH_CRC = 0;
	MCP.statusReg.WRITE = 0;

	passedInfoStruct = MCP391xInfo;
	passedInfoStruct = &MCP;
	return(EXIT_SUCCESS);
}