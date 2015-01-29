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
#include <plib.h>
#include "DMA_Transfer.h"

void SPI_Write_Register(uint8_t registerAddress, uint32_t registerData);
void SPI_Read__Result_Registers(void);
void SPI_Read_All(void);
void SPI_Write_All(void);

MCP391x_Info *passedInfoStruct;
MCP391x_Info MCP;
uint8_t adcBuf[13] = {};
int i;
int j;

/**
 * @brief Sets up the MCP391x.
 * @param MCP391x_Params A pointer to a MCP391x_Info struct which will be updated.
 * @return Returns EXIT_SUCCESS if the device responds with the set configuration.
 */
uint8_t MCP391x_Init(MCP391x_Info *MCP391xInfo)
{
	//Setting bits for configuration of the MCP3912;
	// CLK = 16MHZ
	// DataRate = 16MHZ / 4 / PRE / OSR;
	// OSR = 64, PRE = 1  -->  DataRate = 62.5kspS
	// For OSR >= 256, 3dB rolloff for Sinc filter is .29 * 62.5kspS
	//	so: 18.125 kHz Effective BW
	MCP.config0Reg.BOOST = 0b11;
	MCP.config0Reg.DITHER = 0b11;
	MCP.config0Reg.EN_GAINCAL = 0;
	MCP.config0Reg.EN_OFFCAL = 0;
	MCP.config0Reg.OSR = 0b010; //OSR = 128
	MCP.config0Reg.PRE = 0b00; // 0b11 = 8, 0b10 = 4, 0b01 = 2, 0 = 1
	MCP.config0Reg.VREFCAL = 64;

	MCP.config1Reg.CLKEXT = 0; //This should be set to zero for oscillator :: TESTING
	MCP.config1Reg.RESET = 0b0000;
	MCP.config1Reg.SHUTDOWN = 0b0000;
	MCP.config1Reg.VREFEXT = 0;

	MCP.phaseReg.PHASEA = 0;
	MCP.phaseReg.PHASEB = 0;

	MCP.gainReg.PHA_CH0 = 0;
	MCP.gainReg.PHA_CH1 = 0;
	MCP.gainReg.PHA_CH2 = 0;
	MCP.gainReg.PHA_CH3 = 0;

	MCP.modReg.wholeRegister = 0;

	MCP.statusReg.WIDTH_DATA = 0b00;
	MCP.statusReg.DR_HIZ = 1;
	MCP.statusReg.DR_LINK = 1;
	MCP.statusReg.EN_CRCCOM = 0;
	MCP.statusReg.EN_INT = 0;
	MCP.statusReg.READ = 0b11;
	MCP.statusReg.WIDTH_CRC = 0;
	MCP.statusReg.WRITE = 1;

	SPI_Write_All();
	//SPI_Read_All();  //For Debugging

	passedInfoStruct = MCP391xInfo;
	*passedInfoStruct = MCP;

	// Setting up tx vectors.
	adcBuf[0] = 0x40 | (CHANNEL0_DATA_ADDR << 1);
	adcBuf[0] |= 1 << 0; //Clear R/W* bit for Write operation.


	return(EXIT_SUCCESS);
}

/****************************** PRIVATE FCNS **********************************/

/**
 * The MCP3912 has a read/write sequence which is 32 bits long.  The first 8
 * bits make up the control byte with form:
 *
 * - A 7:6 Device Address:	Default 01
 * - A 5:1 Register Address:	See XXXXX_ADDR defines
 * - A 0 Read/Write		0 = Write, 1 = Read
 *
 * @param registerAddress
 * @param registerData
 */
void SPI_Write_Register(uint8_t registerAddress, uint32_t registerData)
{
	static uint8_t txBuf[4];
	//Toggle SS pin.
	SPI_SS_LAT = 0;

	txBuf[0] = 0x40 | (registerAddress << 1);
	txBuf[0] &= ~(1 << 0); //Clear R/W* bit for Write operation.
	txBuf[1] = (registerData & 0x000000FF);
	txBuf[2] = (registerData & 0x0000FF00) >> 8;
	txBuf[3] = (registerData & 0x00FF0000) >> 16;


	DmaChnClrEvFlags(DMA_CHANNEL1, DMA_EV_BLOCK_DONE);
	BufferToSpi_Transfer(txBuf, 4);


	while (!(DmaChnGetEvFlags(DMA_CHANNEL1) & DMA_EV_BLOCK_DONE)
		|| !(SpiChnTxBuffEmpty(SPI_CHANNEL1)));
	for (i = 0; i < 2; i++) {
		j++;
	}
	SPI_SS_LAT = 1;
}

void SPI_Read_Result_Registers(void)
{
	//Toggle SS pin.
	SPI_SS_LAT = 0;

	DmaChnClrEvFlags(DMA_CHANNEL1, DMA_EV_BLOCK_DONE);
	BufferToSpi_Transfer(adcBuf, 9); //9

	while (!(DmaChnGetEvFlags(DMA_CHANNEL1) & DMA_EV_BLOCK_DONE)
		|| !(SpiChnTxBuffEmpty(SPI_CHANNEL1)));
	for (i = 0; i < 2; i++) {
		j++;
	}
	SPI_SS_LAT = 1;
}

void SPI_Read_All(void)
{
	static uint8_t txBuf[(3 * 15) + 1];
	//Toggle SS pin.
	SPI_SS_LAT = 0;

	txBuf[0] = 0x40 | (0x08 << 1);
	txBuf[0] |= 1 << 0; //Clear R/W* bit for Write operation.
	int i;

	for (i = 1; i < (3 * 15); i++) {
		txBuf[i] = 0;
	}

	DmaChnClrEvFlags(DMA_CHANNEL1, DMA_EV_BLOCK_DONE);
	BufferToSpi_Transfer(txBuf, (3 * 15) + 1);

	while (!(DmaChnGetEvFlags(DMA_CHANNEL1) & DMA_EV_BLOCK_DONE)
		|| !(SpiChnTxBuffEmpty(SPI_CHANNEL1)));
	for (i = 0; i < 2; i++) {
		j++;
	}
	SPI_SS_LAT = 1;
}

void SPI_Write_All(void)
{
	static uint8_t txBuf[(3 * 15) + 1];
	//Toggle SS pin.
	SPI_SS_LAT = 0;
	int i = 1;
	int j = 1;

	txBuf[0] = 0x40 | (0x08 << 1);
	txBuf[0] &= ~(1 << 0); //Clear R/W* bit for Write operation.

	txBuf[3] = MCP.modReg.wholeRegister;
	txBuf[2] = MCP.modReg.wholeRegister >> 8;
	txBuf[1] = MCP.modReg.wholeRegister >> 16;

	txBuf[6] = (MCP.phaseReg.wholeRegister & 0x000000FF);
	txBuf[5] = (MCP.phaseReg.wholeRegister & 0x0000FF00) >> 8;
	txBuf[4] = (MCP.phaseReg.wholeRegister & 0x00FF0000) >> 16;

	txBuf[9] = (MCP.gainReg.wholeRegister & 0x000000FF);
	txBuf[8] = (MCP.gainReg.wholeRegister & 0x0000FF00) >> 8;
	txBuf[7] = (MCP.gainReg.wholeRegister & 0x00FF0000) >> 16;

	txBuf[12] = (MCP.statusReg.wholeRegister & 0x000000FF);
	txBuf[11] = (MCP.statusReg.wholeRegister & 0x0000FF00) >> 8;
	txBuf[10] = (MCP.statusReg.wholeRegister & 0x00FF0000) >> 16;

	txBuf[15] = (MCP.config0Reg.wholeRegister & 0x000000FF);
	txBuf[14] = (MCP.config0Reg.wholeRegister & 0x0000FF00) >> 8;
	txBuf[13] = (MCP.config0Reg.wholeRegister & 0x00FF0000) >> 16;

	txBuf[18] = (MCP.config1Reg.wholeRegister & 0x000000FF);
	txBuf[17] = (MCP.config1Reg.wholeRegister & 0x0000FF00) >> 8;
	txBuf[16] = (MCP.config1Reg.wholeRegister & 0x00FF0000) >> 16;

	DmaChnClrEvFlags(DMA_CHANNEL1, DMA_EV_BLOCK_DONE);
	BufferToSpi_Transfer(txBuf, 19);


	while (!(DmaChnGetEvFlags(DMA_CHANNEL1) & DMA_EV_BLOCK_DONE)
		|| !(SpiChnTxBuffEmpty(SPI_CHANNEL1)));
	for (i = 0; i < 2; i++) {
		j++;
	}
	SPI_SS_LAT = 1;
}

void __ISR(_CHANGE_NOTICE_VECTOR) CNInterrupt(void)
{
	uint32_t temp;
	SPI_Read_Result_Registers();
	temp = mPORTFRead();
	mCNClearIntFlag();
}