/*
 *  Henry Crute
 * 	hcrute@ucsc.edu
 * 	
 * Implementation of the buffers for receiving, and circular buffer
 * for sending
 * 
 */

#include "c_types.h"
#include "osapi.h"
#include "buffers.h"
#include "nmea0183.h"

//the preamble of the post request
#define POST_REQUEST "POST /api/event-api/ HTTP/1.1\r\n"\
					 "User-Agent: ESP8266\r\n"\
					 "Host: seads.brarbsmit.com\r\n"\
					 "Accept: */*\r\n"\
					 "Content-Type: application/json\r\n"\
					 "Authorization: Token 0d1e0f4b56e4772fdb440abf66da8e2c1df799c0\r\n"\
					 "Content-Length: %u\r\n\r\n%s"
//format string for json data
#define JSON_DATA "{\"device\": \"/api/device-api/%u/\", \"wattage\":\"%f\", \"timestamp\":\"%s\"}"

uint8_t device_id = 2;

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
	.buffer[0] = 0,
	.buffer_end = max_send_buff_size - 1,
	.capacity = max_send_buff_size,
	.count = 0,
	.head = 0,
	.tail = 0
};

/******************************UART BUFFER*****************************/
void ICACHE_FLASH_ATTR
init_buffers(void) {
	os_memset(send_buffer.buffer, 0, sizeof(send_data_t) * max_send_buff_size);
}

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
		uart_buffer = &uart_buffer2;
	} else {
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
	uint8_t i;
	uart_tx_one_char('\r');
	uart_tx_one_char('\n');
	for (i = 0; i < uart_buffer->buff_size; i++) {
		uart_tx_one_char(uart_buffer->buffer[i]);
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
	//insert null plug for string manipulation
	uart_buffer->buffer[uart_buffer->write] = 0;
	//checks the uart buffer, strict OFF for now
	//that means it doesn't require a checksum
	if (check(uart_buffer->buffer, false)) {
		return true;
	} else {
		return false;
	}
}

/******************************SEND BUFFER*****************************/

/**
  * @brief  puts the data found in the other uart buffer into the send buffer
  * @param  None
  * @retval returns false if failed to get fields, and true if succeeded
  */
/*
bool ICACHE_FLASH_ATTR
push_send_buffer(void) {
	//initialize temp pointer to the buffer not being used by receive
	uart_buffer_t *temp_ptr = NULL;
	char *buff_ptr = NULL;
	if (uart_buffer == &uart_buffer1) {
		temp_ptr = &uart_buffer2;
	} else {
		temp_ptr = &uart_buffer1;
	}
	//check preamble for talker type, and message type
	talker_id talker = get_talker(temp_ptr->buffer);
	sentence_id sentence = get_sentence(temp_ptr->buffer);
	if (talker == TALKER_SEAD) {
		uart0_sendStr("talker sead...\r\n");
		switch (sentence) {
		case SENTENCE_DAT:
			//process data fields
			//add 8 to the pointer to skip talker id, and message type
			buff_ptr = temp_ptr->buffer + 8;
			send_buffer.buffer[send_buffer.head].wattage = atof(buff_ptr);
			while (*buff_ptr != ',') {
				buff_ptr++;
			}
			buff_ptr++;
			//TODO fix the hardcoded 13 to the length of the actual time
			//given
			//copies timestamp to send buffer timestamp
			os_strncpy(send_buffer.buffer[send_buffer.head].timestamp, buff_ptr, 13);
			
			//TODO FIX PUSH INCRIMENTING
			send_buffer.head++;
			if (send_buffer.capacity == send_buffer.count) {
				//overwrite the oldest item at the tail
				send_buffer.tail++;
				if (send_buffer.tail > send_buffer.tail) {
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
			break;
		case SENTENCE_UNKNOWN:
			uart0_sendStr("sentence unknown...\r\n");
			break;
		default:
			return false;
			break;
		}
	} else if (talker == TALKER_UNKNOWN) {
		uart0_sendStr("talker unknown...\r\n");
		return false;
	}
}
*/
/**
  * @brief  retreives data, makes the format string, sends json data
  * @param  None
  * @retval returns false if failed to send and pop,
  * and true if succeeded
  */
/*
bool ICACHE_FLASH_ATTR
send_pop_buffer(void) {
	//return false if there was nothing on the buffer
	if (send_buffer.count == 0) {
		return false;
	}
	//establish tcp connection with seads.brabsmit.com on port 80
	//if failed, return false
	//else format the output json data into a string
	//initialize buffer to use for sending data
	char json_data[512] = "";
	char send_data[1024] = "";
	uint16_t chars_written = 0;
	chars_written = os_sprintf(json_data, JSON_DATA, device_id,
		send_buffer.buffer[send_buffer.tail].wattage,
		send_buffer.buffer[send_buffer.tail].timestamp);
	//if sprintf failed
	if (chars_written < 0) {
		return false;
	}
	chars_written = os_sprintf(send_data, POST_REQUEST, chars_written,
		json_data);
	//if sprintf failed
	if (chars_written < 0) {
		return false;
	}
	//send the "send_data" and close the tcp connection afterwards
	uart0_sendStr(send_data);
	//decriment the tail and the number of things in the circular buffer
	send_buffer.count--;
	if (send_buffer.tail == 0) {
		send_buffer.tail = send_buffer.buffer_end;
	} else {
		send_buffer.tail--;
	}
	
}
*/
/**
  * @brief  gets the size of the send buffer
  * @param  None
  * @retval returns the current size of the circular buffer
  */
  uint8_t ICACHE_FLASH_ATTR
  size_send_buffer(void) {
	  return send_buffer.count;
  }
  


