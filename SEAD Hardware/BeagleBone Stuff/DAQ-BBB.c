/*
 * BeagleBone DAQ Glue
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <signal.h>

#define ARRAY_SIZE 1024

static void pabort(const char *s)
{
	perror(s);
	abort();
}

static int keepRunning = 1;

void intHandler(int dummy) {
	keepRunning = 0;
}

static const char *device = "/dev/spidev1.0";
static uint8_t mode = 3;
static uint8_t bits = 8;
static uint32_t speed = 1000000;
static uint16_t delay;
uint8_t *tx;
uint8_t *rx;
FILE *output;


static void transfer(int fd)
{
	int ret;
	
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = ARRAY_SIZE,
		.delay_usecs = delay,
		.speed_hz = 0,
		.bits_per_word = 0,
	};
	
	//ioctl can create some subtle data corruptions with
	//.len greater than 1024 bytes.  For this reason we'll just
	// call it multiple times to handle the buffer size.
	uint8_t i;
	for(i = 0; i < 8; i++) {
		ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
		if (ret == 1) {
			pabort("can't send spi message");
		}
		
		for (ret = 0; ret < ARRAY_SIZE; ret++) {
			if (!(ret % 2))
			fprintf(output, "\r\n");
			fprintf(output, "%02X", rx[ret]);
		}
	}
}

int main(int argc, char *argv[])
{
	int ret = 0;
	int fd;
	int fs;
	int fr;
	
	printf("Starting Acquisition\r\n");
	
        tx = malloc(ARRAY_SIZE * sizeof(uint8_t));
        if(tx == NULL) {
                printf("Dynamic memory allocation failed!");
        }

        rx = malloc(ARRAY_SIZE * sizeof(uint8_t));
        if(rx == NULL) {
                printf("Dynamic memory allocation failed!");
        }
	printf("Dynamic memory allocation complete.");

	/*
	 * Initialization of SPI and Request Pin
	 */
	
	fs = open("/sys/class/gpio/gpio115/value", O_RDONLY | 
O_NONBLOCK);
	fr = open("/sys/class/gpio/gpio14/value", O_WRONLY);
	fd = open(device, O_RDWR);
	output = fopen("/home/root/SEAD/AcquiredDataRaw", "w");
	if (output == NULL) 
		pabort("Can't open file handle.\r\n");
	if (fd < 0)
		pabort("can't open device");

	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	printf("spi mode: %d\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
	
	int lastLevel = '0';
	int currentLevel = '0';
	int one = '1';
	int zero = '0';

	pwrite(fr, &one, 1, 0);
	signal(SIGINT, intHandler);
	
	printf("\r\n Capturing Data!  Ctrl-C to End\r\n");
	fprintf(output, "START\r\n");
	while (keepRunning) {
		pread(fs, &currentLevel, 1, 0);
		if((currentLevel == '1') && (lastLevel == '0')) {
			transfer(fd);
		}

		lastLevel = currentLevel;
	}
	fprintf(output, "END");

	printf("\r\nClosing file handles and freeing memory.\r\n");
	pwrite(fr, &zero, 1, 0);
	close(fr);
	close(fd);
	close(fs);
	fclose(output);
	free(tx);
	return ret;
}

