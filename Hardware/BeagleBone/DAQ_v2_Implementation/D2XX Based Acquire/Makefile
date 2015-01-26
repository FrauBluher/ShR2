CC=gcc
CFLAGS=-Wall -Wextra -L. -lftd2xx -Wl,-rpath /usr/local/lib

APP = largeread

all: main.c
	$(CC) -c lib_crc.c
	ar rvs lib_crc.a lib_crc.o 	
	$(CC) main.c lib_crc.a -o $(APP) $(CFLAGS)
	
clean:
	rm -f *.o ; rm $(APP)
