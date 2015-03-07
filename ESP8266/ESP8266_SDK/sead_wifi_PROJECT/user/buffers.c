/*
 *  Henry Crute
 * 	hcrute@ucsc.edu
 * 	
 * Implementation of the buffers for receiving, and circular buffer
 * for sending
 * 
 */

#include "espmissingincludes.h"
#include "c_types.h"
#include "stdlib.h"
#include "osapi.h"

#include "buffers.h"
#include "nmea0183.h"
#include "uart.h"
#include "http_request.h"

//inits the uart buffers! using two semiphores for simultaneous
//read store
uart_buffer_t uart_buffer1 = {
	.buffer[0] = 0,
	.buff_size = 0,
	.read = 0,
	.write = 0
};

uart_buffer_t uart_buffer2 = {
	.buffer[0] = 0,
	.buff_size = 0,
	.read = 0,
	.write = 0
};

uart_buffer_t *uart_buffer = &uart_buffer1;

//inits the sending circular buffer, if you push on the buffer and it is
//full, then we overwrite the oldest values
circular_send_buffer_t send_buffer = {
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
	uart_buffer->read = 0;
	uart_buffer->write = 0;
	uart_buffer->buff_size = 0;
}

/**
  * @brief  swaps out the current buffer with the another one
  * @param  None
  * @retval None
  */
void ICACHE_FLASH_ATTR
swap_buffer(void) {
	if (uart_buffer == &uart_buffer1) {
		//uart0_sendStr("now using 2");
		uart_buffer = &uart_buffer2;
	} else {
		//uart0_sendStr("now using 1");
		uart_buffer = &uart_buffer1;
	}
}

/**
  * @brief  prints the buffer
  * @param  None
  * @retval None
  */
void ICACHE_FLASH_ATTR
print_buffer(void) {
	uart0_sendStr("\r\n");
	uart0_sendStr(uart_buffer->buffer);
	uart0_sendStr("\r\n");
}

/**
  * @brief  puts a char onto the buffer, incriments the write pointer
  * @param  character to put into buffer
  * @retval success or failure
  */
bool ICACHE_FLASH_ATTR
put_buffer(uint8_t character) {
	//if the index is greater than the max buff size minus null plug
	if (uart_buffer->write > (max_uart_buff_size - 1)) {
		return false;
	} else {
		//incriment size, write character, incriment pointer, return
		uart_buffer->buff_size++;
		uart_buffer->buffer[uart_buffer->write++] = character;
		//puts null plug at the end of the buffer
		uart_buffer->buffer[uart_buffer->write] = 0;
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
	if (uart_buffer->buff_size == 0 || uart_buffer->read == uart_buffer->write) {
		return 0;
	} else {
		//decrement size, return value, incriment pointer
		uart_buffer->buff_size--;
		return uart_buffer->buffer[uart_buffer->read++];
	}
}

/**
  * @brief  nmea checksums the buffer string to be validated and stored
  * @param  None
  * @retval returns false if failed, and true if succeeded
  */
bool ICACHE_FLASH_ATTR
checksum_buffer(void) {
	//checks the uart buffer, strict OFF for now
	//that means it doesn't require a checksum
	uart0_sendStr("checksumming:\r\n");
	uart0_sendStr(uart_buffer->buffer);
	if (check(uart_buffer->buffer, false)) {
		return true;
	} else {
		return false;
	}
	uart0_sendStr(uart_buffer->buffer);
}

/******************************SEND BUFFER*****************************/

/**
  * @brief  puts the data found in the other uart buffer into the send buffer
  * @param  None
  * @retval returns false if failed to get fields, and true if succeeded
  */

bool ICACHE_FLASH_ATTR
push_send_buffer(void) {
	os_printf("Push:\r\nhead = %d\r\ntail = %d\r\ncount = %d\r\n",
		send_buffer.head, send_buffer.tail, send_buffer.count);
	//initialize temp pointer to the buffer not being used by receive
	uart_buffer_t *temp_ptr = NULL;
	bool return_value = true;
	//compares the pointers
	if (uart_buffer == &uart_buffer1) {
		temp_ptr = &uart_buffer2;
	} else {
		temp_ptr = &uart_buffer1;
	}
	//if it couldn't find fields then pushing fails
	if (!process_message(temp_ptr->buffer,
					&send_buffer.buffer[send_buffer.head].wattage,
					send_buffer.buffer[send_buffer.head].timestamp)) {
		return false;				
	}
	//incriment if successfully processed message and copied values over
	send_buffer.head++;
	if (send_buffer.capacity == send_buffer.count) {
		//overwrite the oldest item at the tail by incrimenting
		send_buffer.tail++;
		if (send_buffer.tail > send_buffer.buffer_end) {
			send_buffer.tail = 0;
		}
		if (send_buffer.head > send_buffer.buffer_end) {
			send_buffer.head = 0;
		}
	} else {
		//if the head goes off the end, reset the head
		if (send_buffer.head > send_buffer.buffer_end) {
			send_buffer.head = 0;
		}
		send_buffer.count++;
	}
	os_printf("Push:\r\nhead = %d\r\ntail = %d\r\ncount = %d\r\n",
		send_buffer.head, send_buffer.tail, send_buffer.count);
	return return_value;
}

/**
  * @brief  retreives data, makes the format string, sends json data
  * @param  None
  * @retval returns false if failed to send and pop,
  * and true if succeeded
  */

bool ICACHE_FLASH_ATTR
send_pop_buffer(void) {
	os_printf("Pop:\r\nhead = %d\r\ntail = %d\r\ncount = %d\r\n",
		send_buffer.head, send_buffer.tail, send_buffer.count);
	//return if there was nothing on the buffer
	if (send_buffer.count == 0) {
		return false;
	}
	//establish tcp connection with seads.brabsmit.com on port 80
	if (!send_http_request(&send_buffer.buffer[send_buffer.tail])) {
		return false;
	}
	//if initializing the request was successful, return true
	return true;
}

/**
  * @brief  increases the tail
  * @param  None
  * @retval returns false if failed to send and pop,
  * and true if succeeded
  */

void ICACHE_FLASH_ATTR
pop_pop_buffer(void) {
	if (send_buffer.count == 0) {
		return;
	}
	//decriment the count
	//incriment the tail
	send_buffer.count--;
	if (send_buffer.tail > send_buffer.buffer_end) {
		send_buffer.tail = 0;
	} else {
		send_buffer.tail++;
	}
	os_printf("Pop:\r\nhead = %d\r\ntail = %d\r\ncount = %d\r\n",
		send_buffer.head, send_buffer.tail, send_buffer.count);
}

/**
  * @brief  gets the size of the send buffer
  * @param  None
  * @retval returns the current size of the circular buffer
  */
  uint8_t ICACHE_FLASH_ATTR
  size_send_buffer(void) {
	  return send_buffer.count;
  }
  


