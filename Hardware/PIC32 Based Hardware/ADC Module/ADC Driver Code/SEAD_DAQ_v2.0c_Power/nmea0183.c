/**
 * @file	nmea0183.c
 * @author 	Henry Crute
 * @date	February 2015
 * @brief 	Creation of nmea messages. You must free the returned
 *		handle yourself
 */

#include <string.h>
#include <stdlib.h>

#include "nmea0183.h"

//max size for buffer size
#define max_uart_buff_size 255

/**
  * @brief  creates the nmea message to send to the ESP
  * @param  talker ID, type ID, and then number of additional arguments
  * @retval a handle to the created malloced string, or NULL if failed
  */
char *create_message(char *talker, char *message_type, uint8_t arg_num, ...)
{
	va_list valist;
	uint8_t i;
	uint32_t number;
	char buffer[max_uart_buff_size] = {0};
	//start of a message
	buffer[0] = '$';
	strcat(buffer, talker);
	strcat(buffer, message_type);
	//initialize valist for arg_num number of arguments
	va_start(valist, arg_num);
	//access all the arguments in the valist
	for (i = 0; i < arg_num; i++) {
		//adds all number of things
		number = va_arg(valist, uint32_t);
		sprintf(buffer + strlen(buffer), ",%u", number);
	}
	//end of message
	strcat(buffer, "*");
	//appends the checksum to the end of the message
	sprintf(buffer + strlen(buffer), "%x\r\n", checksum(buffer));
	va_end(valist);
	return strdup(buffer);
}

/**
  * @brief  frees created message
  * @param  pointer to free
  * @retval success or failure
  */
bool destroy_message(char *message)
{
	if (message == NULL) {
		return false;
	}
	free(message);
	return true;
}

/**
  * @brief  nmea string checksum function
  * @param  string input to check
  * @retval checksum output XOR of all the bytes in the sentence, not
  * including the initial dollar sign
  */
uint8_t checksum(const char *string)
{
	uint8_t check = 0;
	//skips the dollar sign if present
	if (*string == '$') {
		string++;
	}
	//XOR's everything until end of string, or asterisk
	while(*string && *string != '*') {
		check ^= *string++;
	}
	return check;
}

/**
  * @brief  nmea string checking function
  * @param  input string to check, bool to indicate if string MUST have
  * a checksum at the end
  * @retval true if strict on and checksum matches
  * true if strict is off and checksum matches, or checksum not present
  * false otherwise
  */
bool check(const char *string, bool strict)
{
	uint8_t check = 0;
	uint8_t sentence_len = 0;
	//sentence starts with "$"
	if (*string++ != '$') {
		return false;
	}
	//XOR's everything until end of string, or asterisk
	while(*string && *string != '*') {
		check ^= *string++;
		sentence_len++;
	}
	//if the length is less than 6 then it can't be a message (no fields)
	if (sentence_len < 6) {
		return false;
	}
	if (*string == '*') {
		string++;
		//get the expected checksum
		uint16_t expected = strtol(string, NULL, 16);
		//incriment past the checksum
		string += 2;
		if (expected == 0) {
			return false;
		}
		else if (check != expected) {
			return false;
		}
	//returns false when there is no checksum in strict mode
	} else if (strict) {
		return false;
	}
	//reuturn false if not delimited with \r\n or \n
	if (*string && strcmp(string, "\n") && strcmp(string, "\r\n")) {
		return false;
	}
	return true;
}
