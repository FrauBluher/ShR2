/*
 * Henry Crute
 * hcrute@ucsc.edu
 * 	
 * header file for the character buffer, and data buffer
 * character buffer is size 255, and fifo
 * data buffer is subject to change
 * 
 */

#ifndef __BUFFERS_H
#define __BUFFERS_H

#define max_uart_buff_size 255
//malloc goes to the heap, and so does this (be careful)
#define max_send_buff_size 200

//character buffer for uart
typedef struct {
	char buffer[max_uart_buff_size];
	uint8_t buff_size;
	uint8_t read;
	uint8_t write;
}uart_buffer_t;

//exact data to populate
typedef struct {
	uint16_t wattage;
	uint16_t vrms;
	uint16_t irms;
	char timestamp[14];
}send_data_t;

//circular buffer for sending
typedef struct {
    send_data_t buffer[max_send_buff_size];     // data buffer
    uint16_t buffer_end; 		// end of data buffer
    uint16_t capacity;  		// maximum number of items in the buffer
    uint16_t count;     		// number of items in the buffer
    uint16_t head;      		// pointer to head
    uint16_t tail;       		// pointer to tail
} circular_send_buffer_t;


//prototype definitions start for uart buffer 

void init_buffers(void);
//resets the buffer read and write pointers
void reset_buffer(void);
//swaps out buffer 1 with buffer 2, or the other way
void swap_buffer(void);

//fifo push and pop
bool put_buffer(uint8_t);
uint8_t read_buffer(void);

//debug print buffer
void print_buffer(void);

//nmea checksums the buffer
bool checksum_buffer(void);

//prototype definitions start for send buffer

//push and pop for send buffer
bool push_send_buffer(void);
bool send_pop_buffer(void);
void pop_pop_buffer(void);

//returns current size of send buffer
uint8_t size_send_buffer(void);

#endif
