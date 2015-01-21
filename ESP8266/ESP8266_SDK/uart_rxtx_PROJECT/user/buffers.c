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
#include "nmea0183.h"



//inits the uart buffer
uart_buffer_t uart_buffer = {
	.buffer[0] = 0,
	.buff_size = 0,
	.read = 0,
	.write = 0
};

//inits the sending circular buffer
circular_send_buffer_t send_buffer = {
	.buffer[0] = 0,
	.buffer_end = max_send_buff_size - 1,
	.capacity = max_send_buff_size,
	.count = 0,
	.head = 0,
	.tail = 0
};

/******************************UART BUFFER*****************************/
/**
  * @brief  resets read and write pointers for uart buffer
  * @param  None
  * @retval None
  */
void ICACHE_FLASH_ATTR
reset_buffer(void) {
	uart_buffer.read = 0;
	uart_buffer.write = 0;
	uart_buffer.buff_size = 0;
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
	for (i = 0; i < uart_buffer.buff_size; i++) {
		uart_tx_one_char(uart_buffer.buffer[i]);
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
	if (uart_buffer.write > (max_uart_buff_size - 1)) {
		return false;
	} else {
		//incriment size, write character, incriment pointer, return
		uart_buffer.buff_size++;
		uart_buffer.buffer[uart_buffer.write++] = character;
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
	if (uart_buffer.buff_size == 0 || uart_buffer.read == uart_buffer.write) {
		return 0;
	} else {
		//decrement size, return value, incriment pointer
		uart_buffer.buff_size--;
		return uart_buffer.buffer[uart_buffer.read++];
	}
}

/**
  * @brief  nmea checksums the buffer string to be validated and stored
  * @param  None
  * @retval returns false if failed, and true if succeeded
  */
bool ICACHE_FLASH_ATTR
checksum_buffer(void) {
	//insert null plug for string manipulation
	uart_buffer.buffer[uart_buffer.write] = 0;
	//checks the uart buffer, strict OFF for now
	//that means it doesn't require a checksum
	if (check(uart_buffer.buffer, false)) {
		return true;
	} else {
		return false;
	}
}

/******************************SEND BUFFER*****************************/

/**
  * @brief  puts the data found in the uart buffer into the send buffer
  * @param  None
  * @retval returns false if failed, and true if succeeded
  */
bool ICACHE_FLASH_ATTR
put_send_buffer(void) {
	;
}




