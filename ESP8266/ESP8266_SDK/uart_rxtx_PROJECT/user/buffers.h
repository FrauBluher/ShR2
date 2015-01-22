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

#define max_uart_buff_size 255
#define max_send_buff_size 255

//character buffer for uart
typedef struct {
	uint8_t buffer[max_uart_buff_size];
	uint8_t buff_size;
	uint8_t read;
	uint8_t write;
}uart_buffer_t;

//exact data to send
typedef struct {
	uint16_t wattage;
	char timestamp[14];
}send_data_t;

//circular buffer for sending
typedef struct {
    send_data_t buffer[max_send_buff_size];     // data buffer
    uint8_t buffer_end; 		// end of data buffer
    uint8_t capacity;  			// maximum number of items in the buffer
    uint8_t count;     			// number of items in the buffer
    uint8_t head;      			// pointer to head
    uint8_t tail;       		// pointer to tail
} circular_send_buffer_t;


//prototype definitions start for uart buffer 

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

//returns current size of send buffer
uint8_t size_send_buffer(void);

#endif
