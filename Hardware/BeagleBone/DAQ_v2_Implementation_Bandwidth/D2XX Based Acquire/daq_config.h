/**
 * @file	main.c
 * @author 	Henry Crute
 * @date	February, 2015
 * @brief 	Header shared with the DAQ firmware about sampling config
 */

#ifndef _DAQ_CONFIG
#define _DAQ_CONFIG

/*
 * OSR 2:0		Resolution	-3dB Bandwidth
 * 000	32		17			.26*DRCLK
 * 001	64		20			.26*DRCLK
 * 010	128		23			.26*DRCLK
 * 011	256		24			.26*DRCLK	
 * 100	512		24			.26*DRCLK
 * 101	1024	24			.37*DRCLK
 * 110	2048	24			.42*DRCLK
 * 111	4096	24			.43*DRCLK
*/

typedef struct {
	union {
		uint32_t config0Reg;
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
	union {
		uint32_t gainReg;
		struct {
			unsigned PGA_CH0 : 3;
			unsigned PGA_CH1 : 3;
			unsigned PGA_CH2 : 3;
			unsigned PGA_Ch3 : 3;
			unsigned : 24;
		};
	};
}daq_config;
	

#endif
