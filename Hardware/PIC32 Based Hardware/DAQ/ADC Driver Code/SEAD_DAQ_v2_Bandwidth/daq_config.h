/* 
 * File:   daq_config.h
 * Author: aj
 *
 * Created on March 12, 2015, 2:33 PM
 */

#ifndef DAQ_CONFIG_H
#define	DAQ_CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

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

/* 16MHz Clock / 4 / PRE / OSR = DRCLK
 * PRE	OSR	Bandwidth
 * 00	000	32500
 * 00	001	16250
 * 00	010	8125
 * 00	011	4062.5
 * 00	100	2031.25
 * 00	101	1445.3125
 * 00	110	820.3125
 * 00	111	419.921875
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
			unsigned PGA_CH3 : 3;
			unsigned : 24;
		};
	};
}daq_config;

#ifdef	__cplusplus
}
#endif

#endif	/* DAQ_CONFIG_H */

