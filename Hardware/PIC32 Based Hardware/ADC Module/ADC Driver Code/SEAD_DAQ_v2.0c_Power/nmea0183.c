/**
 * @file	nmea0183.c
 * @author 	Henry Crute
 * @date	February 2015
 * @brief 	Creation of nmea messages. You must free the returned
 *		handle yourself
 */

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

/**
  * @brief  creates the nmea message to send to the ESP
  * @param  talker ID, type ID, and then arguments like watt, and time
  * @retval a handle to the created malloced string, or NULL if failed
  */
char *create_message(char *talker, char *type, uint32_t wattage,
		     uint32_t timestamp)
{
	;
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
