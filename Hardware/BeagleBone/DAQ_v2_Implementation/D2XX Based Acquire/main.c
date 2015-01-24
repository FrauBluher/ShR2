#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ftd2xx.h"

#define BUF_SIZE 18008

static char tmpBuff[BUF_SIZE];
static char tmpBuff2[BUF_SIZE];
static char start = 'r';

int main(int argc, char *argv[])
{
	DWORD dwBytesRead;
	DWORD dwBytesWritten;
	DWORD uartRxQueueSize;
	DWORD fifoRxQueueSize;
	DWORD lpdwAmountInTxQueue;
	DWORD lpdwEventStatus;
	FILE * fh;
	FT_HANDLE ftUART;
	FT_HANDLE ftFIFO;
	FT_STATUS ftStatus;
	int iport;
	
	if(argc > 1) {
		sscanf(argv[1], "%d", &iport);
	} else {
		iport = 0;
	}
	
	fh = fopen("/home/pavlo/Desktop/uhoh", "w");
	if(fh == NULL) {
		printf("Cant open source file\n");
		return 1;
	}
		
	ftStatus = FT_Open(1, &ftUART);
	ftStatus = FT_Open(0, &ftFIFO);
	if(ftStatus != FT_OK) {
		/* 
			This can fail if the ftdi_sio driver is loaded
		 	use lsmod to check this and rmmod ftdi_sio to remove
			also rmmod usbserial
		 */
		printf("FT_Open(%d) failed, check that ftdi_sio and"
				"usbserial are unloaded.\r\n", iport);
		return 1;
	} else {
		printf("opened %d okay\r\n", iport);
		fflush(stdout);
	}

	FT_ResetDevice(ftUART);
	FT_ResetDevice(ftFIFO);

	//Here is where I'd put all of the config stuff read into argv[]

	// == DEPRECATED, Probably not needed.
	//FT_SetBaudRate(ftUART, 921600);
	//FT_SetDtr(ftUART);
	//FT_SetRts(ftUART);
	//FT_SetFlowControl(ftUART, FT_FLOW_RTS_CTS, 0, 0);

	ftStatus = FT_SetDataCharacteristics(ftUART, FT_BITS_8, FT_STOP_BITS_1,
		FT_PARITY_NONE);
	if (ftStatus == FT_OK) {
		printf("Set Data Characteristics worked.\r\n");
	} else {
		printf("Set Data Characteristics FAILED.\r\n");
	}
	FT_SetTimeouts(ftUART, 1000, 1000); //1 Second Timeout	
	FT_SetTimeouts(ftFIFO, 1000, 1000); //1 Second Timeout

	//ftStatus = FT_Write(ftUART, &start, 1, &dwBytesWritten);

	while(1) {
		//FT_GetStatus(ftUART, &uartRxQueueSize, &lpdwAmountInTxQueue, &lpdwEventStatus);
		FT_GetStatus(ftFIFO, &fifoRxQueueSize, &lpdwAmountInTxQueue, &lpdwEventStatus);
		FT_Read(ftFIFO, tmpBuff, fifoRxQueueSize, &dwBytesRead);
		//FT_Read(ftUART, tmpBuff2, uartRxQueueSize, &dwBytesRead);

		//fprintf(stdout, "FIFO: %i, UART: %i\r\n", fifoRxQueueSize, uartRxQueueSize);
		if(fifoRxQueueSize > 1) {
			fwrite(tmpBuff, 1, fifoRxQueueSize, fh);
			//fprintf(stdout, "\r\n");
			fflush(fh);
		}
		
		usleep(100);
	}

	//fwrite(tmpBuff, 1, dwBytesRead, fh);
	fclose(fh);
	FT_Close(ftUART);
	
	return 0;
}
