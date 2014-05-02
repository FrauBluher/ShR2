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
 * @file	ADS85x8.c
 * @author 	Pavlo Milo Manovi
 * @date	April, 2014
 * @brief 	This library provides implementation of methods to init and read the ADS85x8.
 *
 */

#include "ADS85x8.h"
#include "ParallelIO.h"
#include <stdint.h>

#define EXIT_SUCCESS 1
#define EXIT_FAILURE 0

static ADS85x8_Info *passedInfoStruct;

/**
 * @brief Sets up the ADS85x8.
 * @param ADS85x8_Params A pointer to a ADS85x8_Info struct which will be updated.
 * @return Returns EXIT_SUCCESS if the device responds with the set configuration.
 */
uint8_t ADS85x8_Init(ADS85x8_Info *DS85x8Info)
{
	ADS85x8_CONFIGREGISTER response;

	DS85x8Info->configRegister.BUSY_INT = BUSY_INT_BUSY;
	DS85x8Info->configRegister.BUSY_POL = BUSY_INT_ACTIVE_HIGH;
	DS85x8Info->configRegister.CLKOUT = CLKOUT_DISABLE;
	DS85x8Info->configRegister.CLKSEL = CLKSEL_INTERNAL;
	DS85x8Info->configRegister.PD_B = PD_B_DISABLE;
	DS85x8Info->configRegister.PD_C = PD_C_DISABLE;
	DS85x8Info->configRegister.PD_D = PD_D_DISABLE;
	DS85x8Info->configRegister.RANGE_A = RANGE_A_4_VREF;
	DS85x8Info->configRegister.RANGE_B = RANGE_B_4_VREF;
	DS85x8Info->configRegister.RANGE_C = RANGE_C_4_VREF;
	DS85x8Info->configRegister.RANGE_D = RANGE_D_4_VREF;
	DS85x8Info->configRegister.READ_EN = READ_EN_CONFIG;
	DS85x8Info->configRegister.REFBUF = REFBUF_ENABLE;
	DS85x8Info->configRegister.REFDAC = REFDAC_3_0V;
	DS85x8Info->configRegister.REF_EN = REF_EN_ENABLE;
	DS85x8Info->configRegister.STBY = STBY_DISABLED;
	DS85x8Info->configRegister.VREF = VREF_3_0;
	DS85x8Info->configRegister.WRITE_EN = WRITE_EN_CONTENT_UPDATE_ENABLED;



	//WAIT OR WHATEVER AND CLOCK STUFF...
	//CHIP SELECT AND WR* LOW
	//WAIT AT LEAST 15ns
	//SEND BITS 31:16 HIGH FOR AT LEAST 5ns
	Parallel_IO_Write((DS85x8Info->configRegister.wholeRegister & 0xFF00) >> 16);
	Parallel_IO_Write((DS85x8Info->configRegister.wholeRegister & 0x00FF));
	//WR* HIGH FOR AT LEAST 15ns
	//SEND BITS 15:0 HIGH FOR AT LEAST 5ns
	// WR* and CS* BOTH GO HIGH

	//READ TWO 16 bit words AND COMPARE TO SET CONFIG
	//IF CONFIG MATCHES, RETURN SUCCESS.

	passedInfoStruct = DS85x8Info;
	return(EXIT_SUCCESS);
}

/**
 * @brief Get sampled data from ADC with current configuration.
 * @return Updates ADS85x8_Info with current information and sets newData to 1.
 */
void ADS85x8_GetSamples(void)
{
	//See page 11 of datasheet for ads8568...
}