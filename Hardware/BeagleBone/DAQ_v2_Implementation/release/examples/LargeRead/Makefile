CC=gcc
CFLAGS=-Wall -Wextra -L. -lftd2xx -Wl,-rpath /usr/local/lib
APP = largeread

all: $(APP)

$(APP): main.c	
	$(CC) main.c -o $(APP) $(CFLAGS)
	
clean:
	rm -f *.o ; rm $(APP)
