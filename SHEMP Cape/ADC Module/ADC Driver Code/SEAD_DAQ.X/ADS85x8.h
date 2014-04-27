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
 * @brief 	This library provides methods to init and read the ADS85x8.
 *
 * This module provides definitions and methods for setting configuration bits on
 * the ADS85x8 series ADCs.  Support for serial communications mode is not intended
 * as of this version of the library, i.e. the target is for a parallel configuration.
 *
 */


#ifndef ADS85XH_H
#define	ADS85XH_H

#include <stdint.h>

/**
 * @brief CONFIGURATION_REGISTER data bit positions.
 *
 * Note that REFDAC is only active in software mode and correspond to the setting
 * of the internal reference DAC, the value ranges from 0x00CC (.5v) to 0x03FF (2.5v)
 * in 2.44mV/LSB or to 0x03FF (3.0v) in 2.92/LSB if bit 13 is high.
 *
 * In hardware mode, when pin 34 is pulled down to ground the input voltage range
 * is set to +/- 4VREF which can be adjusted by REFDAC in the configuration register.
 * In software mode VREF is either 2.5 or 3V, set by bit 13 in the configuration
 * register.
 *
 * When "This bit is (not) active in hardware mode." is stated in the configuration bit
 * description below it means that the bit is either high or low when the ADC is set
 * to be used in the HW configuration.  Configuration bits can only be written in SW
 * mode.
 */
typedef struct {

    union {
        uint32_t wholeRegister;

        struct {
            unsigned WRITE_EN : 1;
            unsigned READ_EN : 1;
            unsigned CLKSEL : 1;
            unsigned CLKOUT : 1;
            unsigned BUSY_INT : 1;
            unsigned BUSY_POL : 1;
            unsigned STBY : 1;
            unsigned RANGE_A : 1;
            unsigned RANGE_B : 1;
            unsigned PD_B : 1;
            unsigned RANGE_C : 1;
            unsigned PD_C : 1;
            unsigned RANGE_D : 1;
            unsigned PD_D : 1;
            unsigned : 2;
            unsigned REF_EN : 1;
            unsigned REFBUF : 1;
            unsigned VREF : 1;
            unsigned : 3;
            unsigned REFDAC : 10;
        };
    };
} ADS85x8_CONFIGREGISTER;

/**
 * @brief WRITE_EN Options.
 * This bit is not active in hardware mode.
 * 0 = Register content update disabled (default)
 * 1 = Register content update enabled
 */
#define WRITE_EN_CONTENT_UPDATE_DISABLED   0
#define WRITE_EN_CONTENT_UPDATE_ENABLED   1

/**
 * @brief READ_EN Options.
 * This bit is not active in hardware mode.
 * 0 = Normal operation (conversion results available on SDO_A)
 * 1 = Configuration Register contents output on SDO_A with next two accesses
 * (READ_EN automatically resets to '0' thereafter)
 */
#define READ_EN_CONV  0
#define READ_EN_CONFIG   1

/**
 * @brief CLKSEL Options.
 * This bit is active in hardware mode.
 * 0 = Normal operation with internal conversion clock; mandatory in hardware mode (default)
 * 1 = External conversion clock applied through pin 34 (XCLK) is used (conversion takes 19
 * clock cycles)
 */
#define  CLKSEL_INTERNAL  0
#define  CLKSEL_EXTERNAL  1

/**
 * @brief CLKOUT Options.
 * This bit is not active in hardware mode.
 * 0 = Normal operation (default)
 * 1 = Internal conversion clock is available at pin 34
 */
#define  CLKOUT_DISABLE  0
#define  CLKOUT_ENABLE  1

/**
 * @brief BUSY/INT Options.
 * 0 = BUSY/INT pin in BUSY mode (default)
1 = BUSY/INT pin in interrupt mode (INT); can only be used if all eight channels are
sampled simultaneously (all CONVST_x tied together)
 */
#define  BUSY_INT_BUSY  0
#define  BUSY_INT_INTERRUPT  1

/**
 * @brief BUSY_POL Options.
 * This bit is active in hardware mode.
 * 0 = BUSY/INT active high (default)
 * 1 = BUSY/INT active low
 */
#define  BUSY_INT_ACTIVE_HIGH  0
#define  BUSY_INT_ACTIVE_LOW  1

/**
 * @brief STBY Options.
 * This bit is not active in hardware mode.
 * 0 = Normal operation (default)
 * 1 = Entire device is powered down (including the internal clock and reference)
 */
#define  STBY_DISABLED  0
#define  STBY_ENABLED  1

/**
 * @brief RANGE_A Options.
 * This bit is not active in hardware mode.
 * 0 = Input voltage range: 4VREF (default)
 * 1 = Input voltage range: 2VREF
 */
#define  RANGE_A_4_VREF  0
#define  RANGE_A_2_VREF  1

/**
 * @brief RANGE_B Options.
 * This bit is not active in hardware mode.
 * 0 = Input voltage range: 4VREF (default)
 * 1 = Input voltage range: 2VREF
 */
#define  RANGE_B_4_VREF  0
#define  RANGE_B_2_VREF  1

/**
 * @brief RANGE_C Options.
 * This bit is not active in hardware mode.
 * 0 = Input voltage range: 4VREF (default)
 * 1 = Input voltage range: 2VREF
 */
#define  RANGE_C_4_VREF  0
#define  RANGE_C_2_VREF  1

/**
 * @brief RANGE_D Options.
 * This bit is not active in hardware mode.
 * 0 = Input voltage range: 4VREF (default)
 * 1 = Input voltage range: 2VREF
 */
#define  RANGE_D_4_VREF  0
#define  RANGE_D_2_VREF  1

/**
 * @brief PD_B Options.
 * This bit is active in hardware mode.
 * 0 = Normal operation (default)
 * 1 = Channel pair B is powered down
 */
#define  PD_B_DISABLE  0
#define  PD_B_ENABLE  1

/**
 * @brief PD_C Options.
 * This bit is active in hardware mode.
 * 0 = Normal operation (default)
 * 1 = Channel pair C is powered down
 */
#define  PD_C_DISABLE  0
#define  PD_C_ENABLE  1

/**
 * @brief PD_D Options.
 * This bit is active in hardware mode.
 * 0 = Normal operation (default)
 * 1 = Channel pair D is powered down
 */
#define  PD_D_DISABLE  0
#define  PD_D_ENABLE  1

/**
 * @brief REF_EN Options.
 * This bit is not active in hardware mode.
 * 0 = Internal reference source disabled (default)
 * 1 = Internal reference source enabled
 */
#define  REF_EN_DISABLE  0
#define  REF_EN_ENABLE  1

/**
 * @brief REFBUF Options.
 * This bit is active in hardware mode if the parallel interface is used.
 * 0 = Internal reference buffers enabled (default)
 * 1 = Internal reference buffers disabled
 */
#define  REFBUF_ENABLE  0
#define  REFBUF_DISABLE  1

/**
 * @brief VREF Options.
 * This bit is active in hardware mode.
 * 0 = Internal reference voltage set to 2.5V (default)
 * 1 = Internal reference voltage set to 3.0V
 */
#define  VREF_2_5  0
#define  VREF_3_0  1

/**
 * @brief REFDAC[9:0] Options. Do not set below .5v!
 * These bits are active in hardware mode.
 * These bits correspond to the settings of the internal reference DAC's.
 * Bit D9 is the MSB of the DAC. Default value is 3FFh (2.5V, nom) see note at
 * top of library header.  Do not set below .5v
 */
#define  REFDAC_0_5V   0x00CC
#define  REFDAC_1_25V  0x01FF
#define  REFDAC_2_5V   0x03FF //Default
#define  REFDAC_3_0V   0x03FF //Default if pin 13 is set.

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Methods~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/**
 * @brief Struct used to hold configuration information and new sampled data.
 * @param configRegister Current configuration settings.
 * @param sampledDataChA0 Sampled data for this channel.
 * @param sampledDataChA1 Sampled data for this channel.
 * @param sampledDataChB0 Sampled data for this channel.
 * @param sampledDataChB1 Sampled data for this channel.
 * @param sampledDataChC0 Sampled data for this channel.
 * @param sampledDataChC1 Sampled data for this channel.
 * @param sampledDataChD0 Sampled data for this channel.
 * @param sampledDataChD1 Sampled data for this channel.
 * @param newData Flag is set high when new data is ready for processing.
 *
 * If disabling channels and using a parallel interface the data found in the
 * sampled data fields of this struct will follow this pattern as eight reads
 * will always occur (this may change with future versions of this library):
 *
 * Channel A disabled, B/C/D enabled.
 * Read count:   1      2      3      4      5      6      7      8
 *            [CH_B0][CH_B1][CH_C0][CH_C1][CH_D0][CH_D1][CH_B0][CH_B1]
 *              new    new    new    new    new    new    old    old
 *
 * Channels B/C/D disabled, A enabled.
 * Read count:   1      2      3      4      5      6      7      8
 *            [CH_A0][CH_A1][CH_A0][CH_A1][CH_A0][CH_A1][CH_A0][CH_A1]
 *              new    new    old    old    old    old    old    old
 *
 */
typedef struct {
    ADS85x8_CONFIGREGISTER configRegister;
    int16_t sampledDataChA0;
    int16_t sampledDataChA1;
    int16_t sampledDataChB0;
    int16_t sampledDataChB1;
    int16_t sampledDataChC0;
    int16_t sampledDataChC1;
    int16_t sampledDataChD0;
    int16_t sampledDataChD1;
    uint8_t newData;
} ADS85x8_Info;

/**
 * @brief Sets up the ADS85x8.
 * @param ADS85x8_Params A pointer to a ADS85x8_Info struct which will be updated.
 * @return Returns EXIT_SUCCESS if the device responds with the set configuration.
 */
uint8_t ADS85x8_Init(ADS85x8_Info *DS85x8Info);

/**
 * @brief Get sampled data from ADC with current configuration.
 * @return Updates ADS85x8_Info with current information and sets newData to 1.
 */
void ADS85x8_GetSamples(void);

#endif	/* ADS85XH_H */

