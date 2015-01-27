/*
 *  Henry Crute
 * 	hcrute@ucsc.edu
 * 	
 * 	code for nmea protocol checksums
 * 
 * 
 */

#include "c_types.h"
#include "osapi.h"
#include "nmea0183.h"

/**
  * @brief  nmea string checksum function
  * @param  string input to check
  * @retval checksum output XOR of all the bytes in the sentence, not
  * including the initial dollar sign
  */

uint8_t ICACHE_FLASH_ATTR
checksum(const char *string) {
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
bool ICACHE_FLASH_ATTR
check(const char *string, bool strict) {
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
		//debug getting hex value
		char str[15];
		os_sprintf(str, "e:%d\r\n", expected);
		uart0_sendStr(str);
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

/**
  * @brief  returns sentence and talker types for a string
  * @param  string
  * @retval sentence ID or talker ID
  */
sentence_id ICACHE_FLASH_ATTR
get_sentence(const char *string) {
	if (os_strcmp(string + 3, "DAT")) {
		return SENTENCE_DAT;
	} else {
		return SENTENCE_UNKNOWN;
	}
}


talker_id ICACHE_FLASH_ATTR
get_talker(const char *string) {
	if (os_strcmp(string + 1, "SE")) {
		return TALKER_SEAD;
	} else {
		return TALKER_UNKNOWN;
	}
}

