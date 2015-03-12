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
 * @file	MCP391x.h
 * @author 	Pavlo Milo Manovi
 * @date	April, 2014
 * @brief 	This library provides methods to init and read the MCP391x.
 *
 *
 */


#ifndef ADS85XH_H
#define	ADS85XH_H

#define MCP3912

#include <stdint.h>
#include "daq_config.h"

/**
 * @brief MCP3912 ADC Result Data
 */
#define CHANNEL0_DATA_ADDR 0x00
#define CHANNEL1_DATA_ADDR 0x01
#define CHANNEL2_DATA_ADDR 0x02
#define CHANNEL3_DATA_ADDR 0x03

/**
 * @brief CHANNEL0 ADC Data Structure, most significant bit is first.
 */
typedef struct {

    union {
        uint32_t wholeRegister;

        struct {
            signed DATA : 24;
            unsigned : 8;
        };
    };
} MCP391x_ADC_DATA;

/******************************************************************************/

/**
 * @brief Delta-Sigma Modulators Output Configuration Register Address
 */
#define MOD_ADDR 0x08

/**
 * @brief Delta-Sigma Modulators Output Configuration Register Bit-Fields
 *
 * bit 23-16
 *      Unimplemented: read as 0
 * bit 15-12
 *      COMPn_CH3: Comparator Outputs from ADC Channel 3
 * bit 11-8
 *      COMPn_CH2: Comparator Outputs from ADC Channel 2
 * bit 7-4
 *      COMPn_CH1: Comparator Outputs from ADC Channel 1
 * bit 3-0
 *      COMPn_CH0: Comparator Outputs from ADC Channel 0
 *
 */
typedef struct {

    union {
        uint32_t wholeRegister;

        struct {
            unsigned COMP0_CH0 : 1;
            unsigned COMP1_CH0 : 1;
            unsigned COMP2_CH0 : 1;
            unsigned COMP3_CH0 : 1;
            unsigned COMP0_CH1 : 1;
            unsigned COMP1_CH1 : 1;
            unsigned COMP2_CH1 : 1;
            unsigned COMP3_CH1 : 1;
            unsigned COMP0_CH2 : 1;
            unsigned COMP1_CH2 : 1;
            unsigned COMP2_CH2 : 1;
            unsigned COMP3_CH2 : 1;
            unsigned COMP0_CH3 : 1;
            unsigned COMP1_CH3 : 1;
            unsigned COMP2_CH3 : 1;
            unsigned COMP3_CH3 : 1;
            unsigned : 16;
        };
    };
} MCP391x_MOD_REG;

/******************************************************************************/

/**
 * @brief Phase Delay Configuration Register Address
 */
#define PHASE_ADDR 0x0A

/**
 * @brief Phase Delay Configuration Register Bit-Fields
 *
 * bit 23-12
 *      PHASEB<11:0> Phase delay between channels CH2 and CH3 (reference).
 *          Delay = PHASEB<11:0> decimal code/DMCLK
 * bit 11-0
 *      PHASEA<11:0> Phase delay between channels CH0 and CH1(reference).
 *          Delay = PHASEA<11:0> decimal code/DMCLK
 *
 */
typedef struct {

    union {
        uint32_t wholeRegister;

        struct {
            unsigned PHASEA : 12;
            unsigned PHASEB : 12;
            unsigned : 8;
        };
    };
} MCP391x_PHASE_REG;

/******************************************************************************/

/**
 * @brief Gain Configuration Register Address
 */
#define GAIN_ADDR 0x0B

/**
 * @brief PGA Gain Configuration Register
 *
 * bit 23-12
 *      Unimplemented: Read as 0
 * bit 11-0
 *      PGA_CHn<2:0>: PGA Setting for Channel n
 *          111 = Reserved (Gain = 1)
 *          110 = Reserved (Gain = 1)
 *          101 = Gain is 32
 *          100 = Gain is 16
 *          011 = Gain is 8
 *          010 = Gain is 4
 *          001 = Gain is 2
 *          000 = Gain is 1 (DEFAULT)
 *
 */
typedef struct {

    union {
        uint32_t wholeRegister;

        struct {
            unsigned PGA_CH0 : 3;
            unsigned PGA_CH1 : 3;
            unsigned PGA_CH2 : 3;
            unsigned PGA_CH3 : 3;
            unsigned : 24;
        };
    };
} MCP391x_GAIN_REG;

/******************************************************************************/

/**
 * @brief Status and Communication Register Address
 */
#define STATUSCOM_ADDR 0x0C

/**
 * @brief Status and Communication Register Bit-Field
 *
 * bit 23-22
 *      READ<1:0>: Address counter increment setting for Read Communication
 *          11 = Address counter auto-increments, and loops on the entire register map
 *          10 = Address counter auto-increments, and loops on register TYPES (DEFAULT)
 *          01 = Address counter auto-increments, and loops on register GROUPS
 *          00 = Address is not incremented, and continually reads the same
 *               single-register address
 * bit 21
 *      WRITE: Address counter increment setting for Write Communication
 *          1 = Address counter auto-increments and loops on writable part of
 *              the register map (DEFAULT)
 *          0 = Address is not incremented, and continually writes to the same
 *              single register address
 * bit 20
 *      DR_HIZ: Data Ready Pin Inactive State Control
 *          1 = The DR pin state is a logic high when data is NOT ready
 *          0 = The DR pin state is high-impedance when data is NOT ready (DEFAULT)
 * bit 19
 *      DR_LINK: Data Ready Link Control
 *          1 = Data Ready link enabled. Only one pulse is generated on the DR
 *              pin for all ADC channels corresponding to the data ready pulse
 *              of the most lagging ADC. (DEFAULT)
 *          0 = Data Ready link disabled. Each ADC produces its own data ready
 *              pulse on the DR pin.
 * bit 18
 *      WIDTH_CRC: Format for CRC-16 on communications
 *          1 = 32-bit (CRC-16 code is followed by zeros). This coding is
 *              compatible with CRC implementation in most 32-bit MCUs
 *              (including PIC32 MCUs).
 *          0 = 16 bit (default)
 * bit 17-16
 *      WIDTH_DATA<1:0>: ADC Data Format Settings for all ADCs
 *          11 = 32-bit with sign extension
 *          10 = 32-bit with zeros padding
 *          01 = 24-bit (default)
 *          00 = 16-bit (with rounding)
 * bit 15
 *      EN_CRCCOM: Enable CRC CRC-16 Checksum on Serial communications
 *          1 = CRC-16 Checksum is provided at the end of each communication
 *              sequence (therefore each communication is longer). The CRC-16
 *              Message is the complete communication sequence (see section
 *              Section 6.9).
 *          0 = Disabled
 * bit 14
 *      EN_INT: Enable for the CRCREG interrupt function
 *          1 = The interrupt flag for the CRCREG checksum verification is
 *              enabled. The Data Ready pin (DR) will become logic low and stays
 *               logic low if a CRCREG checksum error happens. This interrupt is
 *              cleared if the LOCK<7:0> value is made equal to the PASSWORD
 *              value (0xA5).
 *          0 = The interrupt flag for the CRCREG checksum verification is
 *              disabled. The CRCREG<15:0> bits are still calculated properly
 *              and can still be read in this mode. No interrupt is generated
 *              even when a CRCREG checksum error happens. (Default)
 * bit 13-12
 *      Reserved: Should be kept equal to 0 at all times
 * bit 11-4
 *      Unimplemented: Read as 0
 * bit 3-0
 *      DRSTATUS<3:0>: Data ready status bit for each individual ADC channel
 *          DRSTATUS<n> = 1 - Channel CHn data is not ready (DEFAULT)
 *          DRSTATUS<n> = 0 - Channel CHn data is ready. The status bit is set back
 *                    to '1' after reading the STATUSCOM register. The status
 *                    bit is not set back to '1' by the read of the corresponding
 *                    channel ADC data.
 *
 */
typedef struct {

    union {
        uint32_t wholeRegister;

        struct {
            unsigned DRSTATUS : 4;
            unsigned : 10;
            unsigned EN_INT : 1;
            unsigned EN_CRCCOM : 1;
            unsigned WIDTH_DATA : 2;
            unsigned WIDTH_CRC : 1;
            unsigned DR_LINK : 1;
            unsigned DR_HIZ : 1;
            unsigned WRITE : 1;
            unsigned READ : 2;
            unsigned : 8;
        };
    };
} MCP391x_STATUSCOM_REG;

/******************************************************************************/

/**
 * @brief CONFIG0 Register Address
 */
#define CONFIG0_ADDR 0x0D

/**
 * @brief CONFIG0 Register Bit-Field
 *
 * bit 23
 *      EN_OFFCAL: Enables the 24-bit digital offset error calibration.
 *          1 = Enabled. This mode does not add any group delay to the ADC data.
 *          0 = Disabled (DEFAULT)
 * bit 22
 *      EN_GAINCAL: Enables or disables the 24-bit digital gain error
 *                  calibration on all channels
 *          1 = Enabled. This mode adds a group delay on all channels of 24
 *              DMCLK periods. All data ready pulses are delayed by 24 DMCLK
 *              lock periods, compared to the mode with EN_GAINCAL = 0.
 *          0 = Disabled (DEFAULT)
 * bit 21-20
 *      DITHER<1:0>: Control for dithering circuit for idle tone?s cancellation
 *                  and improved THD on all channels
 *          11 = Dithering ON, Strength = Maximum (DEFAULT)
 *          10 = Dithering ON, Strength = Medium
 *          01 = Dithering ON, Strength = Minimum
 *          00 = Dithering turned OFF
 * bit 19-18
 *      BOOST<1:0>: Bias Current Selection for all ADCs (impacts achievable
 *                  maximum sampling speed, see Table 5-2 of MCP3912 Datasheet)
 *          11 = All channels have current x 2
 *          10 = All channels have current x 1 (Default)
 *          01 = All channels have current x 0.66
 *          00 = All channels have current x 0.5
 * bit 17-16
 *      PRE<1:0>: Analog Master Clock (AMCLK) Prescaler Value
 *          11 = AMCLK = MCLK/8
 *          10 = AMCLK = MCLK/4
 *          01 = AMCLK = MCLK/2
 *          00 = AMCLK = MCLK (Default)
 * bit 15-13
 *      OSR<2:0>: Oversampling Ratio for delta sigma A/D Conversion
 *              (ALL CHANNELS, fD/fS)
 *          111 = 4096 (fd = 244 sps for MCLK = 4 MHz, fs = AMCLK = 1 MHz)
 *          110 = 2048 (fd = 488 sps for MCLK = 4 MHz, fs = AMCLK = 1 MHz)
 *          101 = 1024 (fd = 976 sps for MCLK = 4 MHz, fs = AMCLK = 1 MHz)
 *          100 = 512 (fd = 1.953 ksps for MCLK = 4 MHz, fs = AMCLK = 1MHz)
 *          011 = 256 (fd = 3.90625 ksps for MCLK = 4 MHz, fs = AMCLK = 1 MHz) D
 *          010 = 128 (fd = 7.8125 ksps for MCLK = 4 MHz, fs = AMCLK = 1 MHz)
 *          001 = 64 (fd = 15.625 ksps for MCLK = 4 MHz, fs = AMCLK = 1MHz)
 *          000 = 32 (fd = 31.25 ksps for MCLK = 4 MHz, fs = AMCLK = 1MHz)
 * bit 12-8
 *      Unimplemented: Read as 0
 * bit 7-0
 *      VREFCAL<7:0>: Internal Voltage Temperature coefficient VREFCAL<7:0>
 *                    value. (See Section 5.6.3 ?Temperature compensation
 *                    (VREFCAL<7:0>)? in MCP documentation for complete
 *                    description).
 *
 */
typedef struct {

    union {
        uint32_t wholeRegister;

        struct {
            signed VREFCAL : 8;
            signed : 5;
            signed OSR : 3;
            signed PRE : 2;
            signed BOOST : 2;
            signed DITHER : 2;
            signed EN_GAINCAL : 1;
            signed EN_OFFCAL : 1;
            signed : 8;
        };
    };
} MCP391x_CONFIG0_REG;

/******************************************************************************/

/**
 * @brief CONFIG1 Address Register
 */
#define CONFIG1_ADDR 0x0E

/**
 * @brief CONFIG1 Address Register Bit-Field
 *
 * bit 23-20
 *      Unimplemented: Read as 0.
 * bit 19-16
 *      RESET<3:0>: Soft Reset mode setting for each individual ADC
 *          RESET<n> = 1 : Channel CHn in soft reset mode
 *          RESET<n> = 0 : Channel CHn not in soft reset mode
 * bit 15-12
 *      Unimplemented: Read as 0
 * bit 11-8
 *      SHUTDOWN<3:0>: Shutdown Mode setting for each individual ADC
 *          SHUTDOWN<n> = 1 : ADC Channel CHn in Shutdown
 *          SHUTDOWN<n> = 0 : ADC Channel CHn not in Shutdown
 * bit 7
 *      VREFEXT: Internal Voltage Reference selection bit
 *          1 = Internal Voltage Reference Disabled. An external reference
 *              voltage needs to be applied across the REFIN+/- pins. The analog
 *              power consumption (AI_DD) is slightly diminished in this mode
 *              since the internal voltage reference is placed in Shutdown mode.
 *          0 = Internal Reference enabled. For optimal accuracy, the REFIN+/OUT
 *              pin needs proper decoupling capacitors. REFIN- pin should be
 *              connected to A_GND, when in this mode.
 * bit 6
 *      CLKEXT: Internal Clock selection bit
 *          1 = MCLK is generated externally and should be provided on the OSC1
 *          pin. The oscillator is disabled and uses no current (Default)
 *          0 = Crystal oscillator enabled. A crystal must be placed between
 *              OSC1 and OSC2 with proper decoupling capacitors. The digital
 *              power consumption (DI_DD) is increased in this mode due to the
 *              oscillator.
 * bit 5-0
 *      Unimplemented: Read as 0
 *
 */
typedef struct {

    union {
        uint32_t wholeRegister;

        struct {
            signed : 6;
            signed CLKEXT : 1;
            signed VREFEXT : 1;
            signed SHUTDOWN : 4;
            signed : 4;
            signed RESET : 4;
            signed : 12;
        };
    };
} MCP391x_CONFIG1_REG;


/******************************************************************************/

/**
 * @brief Offset calibration register address.
 */
#define OFFCAL_CH0 0x0F
#define OFFCAL_CH1 0x11
#define OFFCAL_CH2 0x13
#define OFFCAL_CH3 0x15

/**
 * @brief Offset calibration register bit-field.
 *
 * bit 23-0
 *      OFFCAL_CHn:
 *          Digital Offset calibration value for the corresponding channel CHn.
 *          This register is simply added to the output code of the channel bit-
 *          by-bit. This register is 24-bit two's complement MSB first coding.
 *          CHn Output Code = OFFCAL_CHn + ADC CHn Output Code. This register is
 *          a Don't Care if EN_OFFCAL = 0 (Offset calibration disabled), but its
 *          value is not cleared by the EN_OFFCAL bit.
 */
typedef struct {

    union {
        uint32_t wholeRegister;

        struct {
            signed OFFCAL : 24;
            signed : 8;
        };
    };
} MCP391x_OFFCAL_REG;

/******************************************************************************/

/**
 * @brief GAINCAL Register Address
 */
#define GAINCAL_CH0 0x10
#define GAINCAL_CH1 0x12
#define GAINCAL_CH2 0x14
#define GAINCAL_CH3 0x16

/**
 * @brief GAINCAL Register Address Bit-Fields
 *
 * bit 23-0
 *      GAINCAL_CHn: Digital gain error calibration value for the corresponding
 *                   channel CHn. This register is 24-bit signed MSB first
 *                   coding with a range of -1x to +0.9999999x (from 0x800000
 *                   to 0x7FFFFF). The gain calibration adds 1x to this register
 *                   and multiplies it to the output code of the channel bit-by-
 *                   -bit, after offset calibration. The range of the gain
 *                   calibration is thus from 0x to 1.9999999x (from 0x800000 to
 *                   0x7FFFFF). The LSB corresponds to a 2-23 increment in the
 *                   multiplier. CHn Output Code = (GAINCAL_CHn + 1) * ADC CHn
 *                   Output Code. This register is a Don't Care if EN_GAINCAL =
 *                   0 (Gain calibration disabled) but its value is not cleared
 *                   by the EN_GAINCAL bit.
 *
 */
typedef struct {

    union {
        uint32_t wholeRegister;

        struct {
            signed GAINCAL : 24;
            signed : 8;
        };
    };
} MCP391x_GAINCAL_REG;

/******************************************************************************/

/**
 * @brief LOCK and CRC Register Address
 */
#define LOCK_CRC_ADDR 0x1F

/**
 * @brief LOCK and CRC Register Bit-Fields
 *
 * bit 23-16
 *      LOCK<7:0>: Lock Code for the writable part of the register map
 *           LOCK<7:0> = PASSWORD = 0xA5 (Default value): The entire register
 *                       map is writable. The CRCREG<15:0> bits and the CRC
 *                       Interrupt are cleared. No CRC-16 checksum on register
 *                       map is calculated. LOCK<7:0> are different than 0xA5:
 *                       The only writable register is the LOCK/CRC register.
 *                       All other registers will appear as undefined while in
 *                       this mode. The CRCREG checksum is calculated
 *                       continuously and can generate interrupts if the CRC
 *                       Interrupt EN_INT bit has been enabled. If a write to a
 *                       register needs to be performed, the user needs to
 *                       unlock the register map beforehand by writing 0xA5 to
 *                       the LOCK<7:0> bits.
 * bit 15-0
 *      CRCREG<15:0>: CRC-16 Checksum that is calculated with the writable part
 *                    of the register map as a message. This is a read-only
 *                    16-bit code. This checksum is continuously recalculated
 *                    and updated every 16 DMCLK periods. It is reset to its
 *                    default value (0x0000) when LOCK<7:0> = 0xA5.
 *
 */
typedef struct {

    union {
        uint32_t wholeRegister;

        struct {
            signed CRCREG : 16;
            signed LOCK : 8;
            signed : 8;
        };
    };
} MCP391x_LOCK_CRC_REG;

/******************************************************************************/


typedef struct {
    MCP391x_ADC_DATA sampledDataCh0;
    MCP391x_ADC_DATA sampledDataCh1;
    MCP391x_ADC_DATA sampledDataCh2;
    MCP391x_ADC_DATA sampledDataCh3;
    MCP391x_CONFIG0_REG config0Reg;
    MCP391x_CONFIG1_REG config1Reg;
    MCP391x_GAINCAL_REG gainCalCh0Reg;
    MCP391x_GAINCAL_REG gainCalCh1Reg;
    MCP391x_GAINCAL_REG gainCalCh2Reg;
    MCP391x_GAINCAL_REG gainCalCh3Reg;
    MCP391x_GAIN_REG gainReg;
    MCP391x_LOCK_CRC_REG lockReg;
    MCP391x_MOD_REG modReg;
    MCP391x_OFFCAL_REG offCalCh0Reg;
    MCP391x_OFFCAL_REG offCalCh1Reg;
    MCP391x_OFFCAL_REG offCalCh2Reg;
    MCP391x_OFFCAL_REG offCalCh3Reg;
    MCP391x_PHASE_REG phaseReg;
    MCP391x_STATUSCOM_REG statusReg;
    uint8_t newData;
} MCP391x_Info;

/**
 * @brief Sets up the MCP391x
 * @param MCP391xInfo A pointer to a MCP391x_Info struct which will be updated.
 * @return Returns EXIT_SUCCESS if the device responds with the set configuration.
 */
uint8_t MCP391x_Init(MCP391x_Info *MCP391xInfo, daq_config *config);

#endif	/* ADS85XH_H */

