/*
 *  Henry Crute
 * 	hcrute@ucsc.edu
 * 	
 * 	implementation of the buffer
 * 
 */

#include "c_types.h"
#include "osapi.h"
#include "buffers.h"

#define max_buff_size 255

uint8_t uart_buffer[max_buff_size];
uint8_t buff_size = 0;

//read and write pointers for the uart buffer
uint8_t read = 0;
uint8_t write = 0;

/**
  * @brief  resets read and write pointers
  * @param  None
  * @retval None
  */
void ICACHE_FLASH_ATTR
reset_buffer(void) {
	read = 0;
	write = 0;
	buff_size = 0;
}

/**
  * @brief  prints the buffer
  * @param  None
  * @retval None
  */
void ICACHE_FLASH_ATTR
print_buffer(void) {
	uint8_t i;
	uart_tx_one_char('\r');
	uart_tx_one_char('\n');
	for (i = 0; i < buff_size; i++) {
		uart_tx_one_char(uart_buffer[i]);
	}
	uart_tx_one_char('\r');
	uart_tx_one_char('\n');
}

/**
  * @brief  puts a char onto the buffer, incriments the write pointer
  * @param  character to put into buffer
  * @retval success or failure
  */
bool ICACHE_FLASH_ATTR
put_buffer(uint8_t character) {
	//if the index is greater than the max buff size minus null plug
	if (write > (max_buff_size - 1)) {
		return false;
	} else {
		//incriment size, write character, incriment pointer, return
		buff_size++;
		uart_buffer[write++] = character;
		return true;
	}
}

/**
  * @brief  reads a char from the buffer, incriments read pointer
  * @param  None
  * @retval returns 0 if failed, the character if it didn't fail
  */
uint8_t ICACHE_FLASH_ATTR
read_buffer(void) {
	if (buff_size == 0 || read == write) {
		return 0;
	} else {
		//decrement size, return value, incriment pointer
		buff_size--;
		return uart_buffer[read++];
	}
}
