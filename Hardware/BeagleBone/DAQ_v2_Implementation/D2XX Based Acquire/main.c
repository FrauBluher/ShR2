#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include "ftd2xx.h"
#include "lib_crc.h"

#define BUF_SIZE 0x10000

enum {
	BUFFER_A,
	BUFFER_B
};

DWORD dwBytesRead;
DWORD dwBytesWritten;
DWORD fifoRxQueueSize;
DWORD lpdwAmountInTxQueue;
DWORD lpdwEventStatus;

FILE * fh;
FT_HANDLE ftFIFO;
FT_STATUS ftStatus;

int runningTotalA = 0;
int runningTotalB = 0;
int rxCrc = 0xFFFF;
int iport;
int crc;
int i;	

static uint8_t tmpBuff[BUF_SIZE];
static uint8_t tmpBuff2[BUF_SIZE];
static char exit_thread = 0;
static char current_buffer = BUFFER_A;

void *read_fifo(void *pArgs)
{
	(void)pArgs;

	while(exit_thread != 1) {
		if (current_buffer == BUFFER_A) {
			FT_GetStatus(ftFIFO, &fifoRxQueueSize, &lpdwAmountInTxQueue, &lpdwEventStatus);
			FT_Read(ftFIFO, &tmpBuff[runningTotalA], fifoRxQueueSize, &dwBytesRead);
			runningTotalA += dwBytesRead;
		
			if (fifoRxQueueSize > 4000) {
				fprintf(stdout, "ERROR: toRead: %i > 4000.  DATA WILL BE CORRUPTED!\r\n", fifoRxQueueSize);
				fflush(stdout);
			}

			if (runningTotalA >= 26007) {
				current_buffer = BUFFER_B;
			}
		} else if (current_buffer == BUFFER_B) {
			FT_GetStatus(ftFIFO, &fifoRxQueueSize, &lpdwAmountInTxQueue, &lpdwEventStatus);
			FT_Read(ftFIFO, &tmpBuff2[runningTotalB], fifoRxQueueSize, &dwBytesRead);
			runningTotalB += dwBytesRead;

			if (fifoRxQueueSize > 4000) {
				fprintf(stdout, "ERROR: toRead: %i > 4000.  DATA WILL BE CORRUPTED!\r\n", fifoRxQueueSize);
				fflush(stdout);
			}

			if (runningTotalB >= 26007) {
				current_buffer = BUFFER_A;
			}
		}
	}
	(void)FT_Close(ftFIFO);
	
	return NULL;
}


int main(int argc, char *argv[])
{
	pthread_t thread_id;
	int i;
	FT_STATUS ftStatus;
	
	(void)argc; /* Deliberately unused parameter */
	(void)argv; /* Deliberately unused parameter */

	if(argc > 1) {
		sscanf(argv[1], "%d", &iport);
	} else {
		iport = 0;
	}
	
	fh = fopen("/home/pavlo/Desktop/data.txt", "w");
	if(fh == NULL) {
		printf("Cant open source file\n");
		return 1;
	}
		
	ftStatus = FT_Open(0, &ftFIFO);
	if(ftStatus != FT_OK) {
		printf("FT_Open(%d) failed, check that ftdi_sio and"
			"usbserial are unloaded.\r\n Use rmmod.\r\n",
			iport);
		return 1;
	}

	FT_ResetDevice(ftFIFO);
	FT_SetTimeouts(ftFIFO, 1000, 1000); //1 Second Timeout
	
	pthread_create(&thread_id, NULL, &read_fifo, NULL);

	int32_t ch0 = 0;
	int32_t ch1 = 0;
	int32_t ch2 = 0;
	int32_t ch3 = 0;

	while(1) {
		if(runningTotalA >= 26007 && current_buffer == BUFFER_B) {

			for (i = 0; i < 26000; i++) {
				rxCrc = update_crc_ccitt(rxCrc, tmpBuff[i]);
			}

			crc = ((tmpBuff[26006] << 8) | (tmpBuff[26007]));
			fprintf(stdout, "Block transfer complete, TX-CRC:%i,"
				" RX-CRX:%i\r\n", crc, rxCrc);
			fflush(stdout);

/*
			for (i = 0; i < 26000; i++) {
				fprintf(fh, "%02X, i = %i\r\n", tmpBuff[i], i);
			}
*/


			for (i = 0; i < 26000;) {
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

				ch0 = ((ch0 << 8) >> 8);
				ch1 = ((ch1 << 8) >> 8);
				ch2 = ((ch2 << 8) >> 8);
				ch3 = ((ch3 << 8) >> 8);

				fprintf(fh, "%i, %i, %i, %i\r\n", ch0, ch1, ch2, ch3);
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
		} else if (runningTotalB >= 26007 && current_buffer == BUFFER_A) {

			for (i = 0; i < 26000; i++) {
				rxCrc = update_crc_ccitt(rxCrc, tmpBuff2[i]);
			}

			crc = ((tmpBuff2[26006] << 8) | (tmpBuff2[26007]));
			fprintf(stdout, "Block transfer complete, TX-CRC:%i, RX-CRX:%i\r\n", crc, rxCrc);
			fflush(stdout);

/*
			for (i = 0; i < 26000; i++) {
				fprintf(fh, "%02X, i = %i\r\n", tmpBuff2[i], i);
			}
*/


			for (i = 0; i < 26000;) {
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

				ch0 = ((ch0 << 8) >> 8);
				ch1 = ((ch1 << 8) >> 8);
				ch2 = ((ch2 << 8) >> 8);
				ch3 = ((ch3 << 8) >> 8);

				fprintf(fh, "%i, %i, %i, %i\r\n", ch0, ch1, ch2, ch3);
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
	}

	exit_thread = 1;
	fclose(fh);
	
	return 0;
}
