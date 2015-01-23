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
char currChar;
char match;
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
	int i, j, k = 0;
	int closestSpeed;
	int runningTotal = 0;
	int overflowSize = 0;
	int lastSize = 0;
	long txCrc = 0;
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
	tio.c_cflag &= ~CRTSCTS;
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

	while(keepRunning) {
		if (select(serial_fd + 1, &read_fds, NULL, NULL, &timeout) == 1) {

			ioctl(serial_fd, FIONREAD, &retVal);

			for (i = 0; i < retVal; i++) {
				read(serial_fd, &currChar, 1);

				if (currChar == 'E') {
					match = 1;
				} else if (currChar == 'N' && match == 1) {
					match = 2;
				} else if (currChar == 'D' && match == 2) {
					match = 3;
				} else if (currChar == '*' && match == 3) {
					match = 4;
				} else if (match == 4) {
					txCrc |= (currChar & 0xFF000000);
					match = 5;
				} else if (match == 5) {
					txCrc |= (currChar & 0x00FF0000);
					match = 6;
				} else if (match == 6) {
					txCrc |= (currChar & 0x0000FF00);
					match = 7;	
				} else if (match == 7) {
					txCrc |= (currChar & 0x000000FF);
					match = 0;
					runningTotal = 0;
					for (j = 0; j < 18000; j += 9) {
						fprintf(output, "%i,%i,%i,%i\r\n", ((int) (tmpBuff[j+1] << 8) | tmpBuff[j+2]),
							((int) (tmpBuff[j+3] << 8) | tmpBuff[j+4]),
							((int) (tmpBuff[j+5] << 8) | tmpBuff[j+6]),
							((int) (tmpBuff[j+7] << 8) | tmpBuff[j+8]));
					}
					fprintf(stdout,"block complete, crc: %li\r\n", txCrc);
					fflush(stdout);
					fflush(output);

				} else {
					match = 0;
					if (runningTotal > 18000) {
						fprintf(stdout, "uhoh\r\n");
						fflush(stdout);
						runningTotal = 0;
					}
					tmpBuff[runningTotal] = currChar;
					runningTotal++;
				}
			}		
		} else {
			fprintf(stdout, "Timeout.\r\n");
			fflush(stdout);
		}


		timeout.tv_sec = 1; //This needs to be set every time we return from select().	
	}

}
