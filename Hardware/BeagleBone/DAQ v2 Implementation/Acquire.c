#include <asm/termios.h>
#include <fcntl.h>
#include <err.h>
#include <linux/serial.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>

static const char *device = "/dev/ttyUSB1";
static const char *start = "\r";
static const char *outFile= "/home/pavlo/Desktop/uhoh.txt";
static int speed = 921600;
static int fP;
static int keepRunning = 1;
static char tmpBuff[19000];
static char overflow[1000];
FILE *output;

int openDAQ(void)
{
	int fd;  //File Descriptor

	fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);

	if (fd == -1)
	{
		perror("Couldn't open the DAQ's tty!");
	} else {
		fcntl(fd, F_SETFL, 0);
	}

	return(fd);
}

int main(int argc, char *argv[])
{
	int retVal;
	int serial_fd;
	int closestSpeed;
	int runningTotal = 0;
	int overflowSize = 0;
	int lastSize = 0;
	unsigned short txCrc = 0;
	unsigned short crc_ccitt_ffff = 0xFFFF;
	struct termios2 tio;
	struct serial_struct ss;
	struct timeval timeout;

	timeout.tv_sec = 1; //S
	timeout.tv_usec = 0; //uS
	fd_set read_fds; //What we'll use with FD_SET and pass to select.
	
	serial_fd = openDAQ();

	FD_SET(serial_fd, &read_fds);

	//Newer method for setting custom baud rates (kernel 2.6+)
	ioctl(serial_fd, TCGETS2, &tio);
	tio.c_cflag &= ~CBAUD;
//	tio.c_cflag &= ~CRTSCTS;
	tio.c_cflag |= BOTHER;
	tio.c_ispeed = speed;
	tio.c_ospeed = speed;
	/* do other miscellaneous setup options with the flags here */
	ioctl(serial_fd, TCSETS2, &tio);

	output = fopen(outFile, "w");
	if (output == NULL) {
		fprintf(stdout, "Can't open file handle. %s\r\n", outFile);
		fflush(stdout);
	}

	/*********************************** Init Done *******************************/

	// In the future this is be threaded, which is why I'm implememting this with
	// select(), to make sure that reads with ioctl are non-blocking and work to
	// empty the rx-buffer when the CPU gets around to the thread running this pro
	// -gram.
	
	fprintf(stdout, "%i\r\n", write(serial_fd, start, 1));
	fflush(stdout);

	while( keepRunning) {
		if (select(serial_fd + 1, &read_fds, NULL, NULL, &timeout) == 1) {

			ioctl(serial_fd, FIONREAD, &retVal);

			if (retVal + runningTotal > 18008) {
				overflowSize = runningTotal + retVal - 18008;
				lastSize = retVal - overflowSize;
				read(serial_fd, &tmpBuff[runningTotal], lastSize);
				read(serial_fd, &overflow, overflowSize);
				runningTotal += retVal;
				fprintf(stdout, "uhoh! overflow:%i lastSize:%i \r\n", overflowSize, lastSize);
				fflush(stdout);
			} else {
				read(serial_fd, &tmpBuff[runningTotal], retVal);
				runningTotal += retVal;
			}

			if (runningTotal >= 18008) {
				//Calculate CRC now that we have our block of data.
				txCrc =	(tmpBuff[18006] << 8) |
					(tmpBuff[18007]);

				fprintf(stdout, "Transmitted CRC: %i running total: %i\r\n", txCrc, runningTotal);
				fflush(stdout);

				//fwrite(tmpBuff, 1, retVal, output);
				//fflush(output);
				if (runningTotal == 18008) {
					runningTotal = 0;
				} else {
					memcpy(overflow, tmpBuff, overflowSize);
					runningTotal = runningTotal - 18008;
				}
			}

		} else {
			fprintf(stdout, "Timeout.\r\n");
			fflush(stdout);
		}
		





		timeout.tv_sec = 1; //This needs to be set every time we return from select().	
	}

}
