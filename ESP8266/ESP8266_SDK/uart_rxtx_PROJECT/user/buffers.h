/*
 *  Henry Crute
 * 	hcrute@ucsc.edu
 * 	
 * header file for the character buffer, and data buffer
 * character buffer is size 256, and fifo
 * 
 * 
 * 
 */

#ifndef __BUFFERS_H
#define __BUFFERS_H

typedef struct {
	uint16_t wattage;
	char timestamp[14];
}send_data_t;

//prototype definitions start for library

//resets the buffer read and write pointers
void reset_buffer(void);

//fifo push and pop
bool put_buffer(uint8_t);
uint8_t read_buffer(void);

//debug print buffer
void print_buffer(void);


#endif
