/**
 * @file	main.c
 * @author 	Henry Crute
 * @date	February, 2015
 * @brief 	Header shared with the DAQ firmware about sampling config
 */

#ifndef _DAQ_CONFIG
#define _DAQ_CONFIG

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
		uint32_t offCalChXReg;
		struct {
			signed OFFCAL : 24;
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
