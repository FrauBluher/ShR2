/**
 * @file	main.c
 * @author 	Pavlo Milo Manovi, Henry Crute, Andrew Ringer
 * @date	May, 2015
 * @brief 	Main acquire script to read data from the SEAD device.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "ftd2xx.h"
#include "lib_crc.h"
#include "daq_config.h"

#define BUF_SIZE 0x10000
#define BAUD_RATE 115200
#define MAX_CONFIG_SIZE 255

enum {
	BUFFER_A,
	BUFFER_B
};

DWORD dwBytesRead;
DWORD dwBytesWritten;
DWORD fifoRxQueueSize;
DWORD lpdwAmountInTxQueue;
DWORD lpdwEventStatus;
DWORD bytes;

FILE * fh;
FT_HANDLE ftFIFO;
FT_STATUS ftStatus;

int runningTotalA = 0;
int runningTotalB = 0;
int rxCrc = 0xFFFF;
int crc;
int i;

/* Variables associated with getopt */
int iport = 0;
char *outfile = "data.txt";
double bandwidth = 2000;
double frequency = 8000;
int channels = 4;
bool verbose = false;

//variables for acquiring data, and buffer swapping
static uint8_t tmpBuff[BUF_SIZE];
static uint8_t tmpBuff2[BUF_SIZE];
static char exit_thread = 0;
static char current_buffer = BUFFER_A;

//setup e

// file_names

FILE *current_file = NULL;
unsigned int file_seconds = 60 * 5; // 5 minutes

//file thread
//NOTE: if seconds is faster than it takes to write 1 block we might
void *file_timer(void *args) {
    (void) args;
    // assuming we don't make more than 10^10 files (10) + null plug + '_'
    char *file_name = calloc(strlen(outfile)+12, sizeof(char));
    int count = 0;
    for(;;) {
        sleep(file_seconds);
        sprintf(file_name, "%d_%s", ++count, outfile);
        current_file = fopen(file_name, "wb");
    }
    pthread_exit(NULL);
}

//reads from the usb uart fifio
void *read_fifo(void *pArgs)
{
	(void)pArgs;
	while(exit_thread != 1) {
		if (current_buffer == BUFFER_A) {
			FT_GetStatus(ftFIFO, &fifoRxQueueSize, &lpdwAmountInTxQueue, &lpdwEventStatus);
			DWORD size = 0;
            // we have room for the data
            if (runningTotalA + fifoRxQueueSize < BUF_SIZE) {
                size = fifoRxQueueSize;
            }
            // read as much as we can
            else {
                size = BUF_SIZE - runningTotalA;
				fprintf(stderr, "ERROR: Not enough room in buffer for data. DATA MAY BE CORRUPTED!\n");
				fflush(stderr);
            }
            FT_Read(ftFIFO, &tmpBuff[runningTotalA], size, &dwBytesRead);
			runningTotalA += dwBytesRead;
			if (fifoRxQueueSize > 4000) {
				fprintf(stderr, "ERROR: toRead: %i > 4000. DATA WILL BE CORRUPTED!\n", fifoRxQueueSize);
				fflush(stderr);
			}
			if (runningTotalA > 26007) {
				current_buffer = BUFFER_B;
			}
		} else if (current_buffer == BUFFER_B) {
			FT_GetStatus(ftFIFO, &fifoRxQueueSize, &lpdwAmountInTxQueue, &lpdwEventStatus);
            DWORD size = 0;
            // we have room for the data
            if (runningTotalB + fifoRxQueueSize < BUF_SIZE) {
                size = fifoRxQueueSize;
            }
            // read as much as we can
            else {
                size = BUF_SIZE - runningTotalB;
				fprintf(stderr, "ERROR: Not enough room in buffer for data. DATA MAY BE CORRUPTED!\n");
				fflush(stdout);
            }
            FT_Read(ftFIFO, &tmpBuff2[runningTotalB], size, &dwBytesRead);
			runningTotalB += dwBytesRead;
			if (fifoRxQueueSize > 4000) {
				fprintf(stderr, "ERROR: toRead: %i > 4000. DATA WILL BE CORRUPTED!\n", fifoRxQueueSize);
				fflush(stderr);
			}
			if (runningTotalB > 26007) {
				current_buffer = BUFFER_A;
			}
		}
	}
	(void)FT_Close(ftFIFO);
	return NULL;
}

//goes over the main acquire loop
void acquire_loop(daq_config config)
{
	int i;
	//scalars for bin to microamp conversion.
	//double ch0_scalar = 5.435 * ((1.2 * sqrt(2) * 121.9 / -0.5623) / ((1<<24) * (1<<config.PGA_CH0)));
	double ch0_scalar = (1200 * 100 * 1.309 * 0.4528) / ((1<<24) * (1<<config.PGA_CH0) * 0.235);
	double ch1_scalar = (1200 * 20  * 1.309) / ((1<<24) * (1<<config.PGA_CH1) * 0.333);
	double ch2_scalar = (1200 * 100 * 1.309 * 0.9722) / ((1<<24) * (1<<config.PGA_CH2) * 0.235);
	double ch3_scalar = (1200 * 20  * 1.309 * 0.8041) / ((1<<24) * (1<<config.PGA_CH3) * 0.333);
	pthread_t thread_id;
	FT_ResetDevice(ftFIFO);
	FT_SetTimeouts(ftFIFO, 1000, 1000); //1 Second Timeout
	pthread_create(&thread_id, NULL, &read_fifo, NULL);
    current_file = fh;
    pthread_t file_thread;
    if (file_seconds) {
        pthread_create(&file_thread, NULL, &file_timer, NULL);
    }
	int32_t ch0 = 0;
	int32_t ch1 = 0;
	int32_t ch2 = 0;
	int32_t ch3 = 0;
	while(1) {
        fh = current_file;
		if(runningTotalA > 26007 && current_buffer == BUFFER_B) {
			for (i = 0; i < 26000; i++) {
				rxCrc = update_crc_ccitt(rxCrc, tmpBuff[i]);
			}
			crc = ((tmpBuff[26006] << 8) | (tmpBuff[26007]));
			//fprintf(stdout, "Block transfer complete, TX-CRC:%i,"
			//	" RX-CRX:%i\r\n", crc, rxCrc);
			//fflush(stdout);
			for (i = 0; i < 26000;) {
				//shifting from the uart buffer into the file variables
				ch0 |= (tmpBuff[i+2] << 16);
				ch0 |= (tmpBuff[i+3] << 8);
				ch0 |= (tmpBuff[i]);
				ch1 |= (tmpBuff[i+5] << 16);
				ch1 |= (tmpBuff[i+6] << 8);
				ch1 |= (tmpBuff[i+4]);
				ch2 |= (tmpBuff[i+8] << 16);
				ch2 |= (tmpBuff[i+9] << 8);
				ch2 |= (tmpBuff[i+7]);
				ch3 |= (tmpBuff[i+11] << 16);
				ch3 |= (tmpBuff[i+12] << 8);
				ch3 |= (tmpBuff[i+10]);
				//sign extension
				ch0 = ((ch0 << 8) >> 8);
				ch1 = ((ch1 << 8) >> 8);
				ch2 = ((ch2 << 8) >> 8);
				ch3 = ((ch3 << 8) >> 8);
				//milliamps conversion here
				fprintf(fh, "%f, %f, %f, %f\r\n",
						ch0 * ch0_scalar, ch1 * ch1_scalar,
						ch2 * ch2_scalar, ch3 * ch3_scalar);
				//fprintf(fh, "%08X, %08X, %08X, %08X i=%i\r\n", ch0, ch1, ch2, ch3, i);
				i += 13;
				ch0 = 0;
				ch1 = 0;
				ch2 = 0;
				ch3 = 0;
			}
			fflush(fh);
			runningTotalA = 0;
			rxCrc = 0xFFFF;
		} else if (runningTotalB > 26007 && current_buffer == BUFFER_A) {
			for (i = 0; i < 26000; i++) {
				rxCrc = update_crc_ccitt(rxCrc, tmpBuff2[i]);
			}
			crc = ((tmpBuff2[26006] << 8) | (tmpBuff2[26007]));
			//fprintf(stdout, "Block transfer complete, TX-CRC:%i, RX-CRX:%i\r\n", crc, rxCrc);
			//fflush(stdout);
			for (i = 0; i < 26000;) {
				//shifting from the uart buffer into the file variables
				ch0 |=  (tmpBuff2[i+2] << 16);
				ch0 |=  (tmpBuff2[i+3] << 8);
				ch0 |=  (tmpBuff2[i]);
				ch1 |=  (tmpBuff2[i+5] << 16);
				ch1 |=  (tmpBuff2[i+6] << 8);
				ch1 |=  (tmpBuff2[i+4]);
				ch2 |=  (tmpBuff2[i+8] << 16);
				ch2 |=  (tmpBuff2[i+9] << 8);
				ch2 |=  (tmpBuff2[i+7]);
				ch3 |=  (tmpBuff2[i+11] << 16);
				ch3 |=  (tmpBuff2[i+12] << 8);
				ch3 |=  (tmpBuff2[i+10]);
				//sign extension
				ch0 = ((ch0 << 8) >> 8);
				ch1 = ((ch1 << 8) >> 8);
				ch2 = ((ch2 << 8) >> 8);
				ch3 = ((ch3 << 8) >> 8);
				//milliamps conversion here
				fprintf(fh, "%f, %f, %f, %f\r\n",
						ch0 * ch0_scalar, ch1 * ch1_scalar,
						ch2 * ch2_scalar, ch3 * ch3_scalar);
				//fprintf(fh, "%08X, %08X, %08X, %08X i=%i\r\n", ch0, ch1, ch2, ch3, i);
				i += 13;
				ch0 = 0;
				ch1 = 0;
				ch2 = 0;
				ch3 = 0;
			}
			fflush(fh);
			runningTotalB = 0;
			rxCrc = 0xFFFF;
		}
        if (fh != current_file) {
            fclose(fh);
        }
	}
	return;
}

//prints the usage for the program
void print_usage()
{
	printf("Usage: Acquire [-h] [-v] [-p port] [-b bandwidth] [-f file] [-c channels] [-t seconds]\n");
	return;
}

//prints the config from default to changed
void print_config()
{
	printf("iport %d\n", iport);
	printf("sample frequency %f\n", frequency);
	printf("-3dB bandwidth %f\n", bandwidth);
	printf("outfile: %s\n", outfile);
	printf("channels: %d\n", channels);
}

//gets options, and populates global option variables
void get_options(int argc, char **argv)
{
	opterr = 0;
	int c;
	while ((c = getopt (argc, argv, "hvp:b:f:c:t:")) != -1) {
		switch (c)
		{
		//h for help
		case 'h':
			print_usage();
			exit(0);
			break;
		//v for verbose
		case 'v':
			verbose = true;
			break;
		//p for usb port number
		case 'p':
			iport = strtol(optarg, (char **)NULL, 10);
			break;
		//b for sampling frequency
		case 'b':
			bandwidth = strtol(optarg, (char **)NULL, 10);
			break;
		//f for output file
		case 'f':
			outfile = optarg;
			break;
		//c for how many channels **** currently not supported
		case 'c':
			channels = strtol(optarg, (char **)NULL, 10);
			break;
        case 't':
            file_seconds = atoi(optarg);
            break;
		//unknown options
		case '?':
			if (optopt == 'c' || optopt =='f' || optopt == 'b' || optopt == 'p')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint (optopt))
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf (stderr, "Unknown option character `\\x%x'.\n",
                   optopt);
			exit(1);
		default:
			abort ();
		}
	}
	return;
}

//sets the default values in the daq config struct that is sent over
daq_config default_config()
{
	daq_config config;
	//config0 register for the MCP
	config.BOOST = 0b11;
	config.DITHER = 0b11;
	config.EN_GAINCAL = 0;
	config.EN_OFFCAL = 0;
	config.OSR = 0b010; //OSR = 128
	config.PRE = 0b00; //prescalar = 1
	config.VREFCAL = 64;
	//programmable gain amplifier register
	config.PGA_CH0 = 0b000;
	config.PGA_CH1 = 0b000;
	config.PGA_CH2 = 0b000;
	config.PGA_CH3 = 0b000;
	return config;
}

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

//packages config struct from the input parameters
daq_config package_config()
{
	//create a config struct with default values
	daq_config config = default_config();
	//sets oversample ratio to configure bandwidth/sample frequency.
	//this is determined by user input for the bandwidth
	if (bandwidth < 419.921875) {
		config.OSR = 0b111;
		bandwidth = .43 * 976.5625;
		frequency = 976.5625;
	} else if (bandwidth < 820.3125) {
		config.OSR = 0b110;
		bandwidth = .42 * 1953.125;
		frequency = 1953.125;
	} else if (bandwidth < 1445.3125) {
		config.OSR = 0b101;
		bandwidth = .37 * 3906.25;
		frequency = 3906.25;
	} else if (bandwidth < 2031.25) {
		config.OSR = 0b100;
		bandwidth = .26 * 7812.5;
		frequency = 7812.5;
	} else if (bandwidth < 4062.5) {
		config.OSR = 0b011;
		bandwidth = .26 * 15625;
		frequency = 15625;
	} else if (bandwidth < 8125) {
		config.OSR = 0b010;
		bandwidth = .26 * 31250;
		frequency = 31250;
	} else if (bandwidth < 16250) {
		config.OSR = 0b001;
		bandwidth = .26 * 62500;
		frequency = 62500;
	} else {
		config.OSR = 0b000;
		bandwidth = .26 * 125000;
		frequency = 125000;
	}
	return config;
}

//sends the config struct over the ft tx
void send_config(daq_config config)
{
	FT_STATUS ftStatus;
    FT_HANDLE ftUart;
	//opens the device
    ftStatus = FT_Open(1, &ftUart);
    if (ftStatus != FT_OK) {
        fprintf(stderr, "Unable to open device (%d)", (int)ftStatus);
        exit(1);
    }
	//sets baud rate
    ftStatus = FT_SetBaudRate(ftUart, BAUD_RATE);
    if (ftStatus != FT_OK) {
        fprintf(stderr, "Unable to set baud rate (%d)", (int)ftStatus);
        FT_Close(ftUart);
        exit(1);
    }
    // validate that this is big enough for our data
    assert(sizeof(config) < MAX_CONFIG_SIZE);
	//writes config to the uart
    ftStatus = FT_Write(ftUart, &config, (DWORD)sizeof(config), &bytes);
    if (ftStatus != FT_OK || bytes != sizeof(config)) {
        fprintf(stderr, "Unable to send config (%d) size (%d)", (int)ftStatus, bytes);
        FT_Close(ftUart);
        exit(1);
    }
	//writes \r\n to terminate config to uart
    ftStatus = FT_Write(ftUart, "\r\n", 2, &bytes);
    if (ftStatus != FT_OK || bytes != 2) {
        fprintf(stderr, "Unable to send newline (%d) size (%d)", (int)ftStatus, bytes);
        FT_Close(ftUart);
        exit(1);
    }
	//closes sending config
    FT_Close(ftUart);
	return;
}

int main(int argc, char *argv[])
{
	FT_STATUS ftStatus;
	//process options
	get_options(argc, argv);
	//calculate and populate config struct
	daq_config config = package_config();
	//print all configurations
	if (verbose) {
		print_config();
	}
	//file handle
    if (file_seconds) {
        char *out_name = calloc(strlen(outfile)+3, sizeof(char));
        sprintf(out_name, "0_%s", outfile);
	    fh = fopen(out_name, "wb");
    }
    else {
	    fh = fopen(outfile, "wb");
    }
	if(fh == NULL) {
		printf("Cant open source file\n");
		exit(1);
	}
	ftStatus = FT_Open(0, &ftFIFO);
	if(ftStatus != FT_OK) {
		printf("FT_Open(%d) failed, check that ftdi_sio and"
			"usbserial are unloaded.\r\n Use rmmod.\r\n",
			iport);
		exit(1);
	}
	//sends config over to the DAQ
	send_config(config);
	//acquires data
	acquire_loop(config);
	exit_thread = 1;
	fclose(fh);
	exit(0);
}
