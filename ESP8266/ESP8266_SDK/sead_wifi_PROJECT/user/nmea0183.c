/*
 * Henry Crute
 * hcrute@ucsc.edu
 * 	
 * code for nmea protocol checksums
 *  
 */

//for the strtol
#include "stdlib.h"
#include "osapi.h"
//for the os_printf etc
#include "espmissingincludes.h"
#include "c_types.h"
#include "nmea0183.h"
#include "uart.h"

#include "extralib.h"

bool echo = false;
/**
  * @brief  Nmea string checksum function
  * @param  String input to check
  * @retval Checksum output XOR of all the bytes in the sentence, not
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
	const char *string_ptr = string;
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
		DEBUG_PRINT((str, "e:%d\r\n", expected));
		DEBUG_PRINT((str));
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
   if (echo) {
      DEBUG_PRINT((string_ptr));
   }
	return true;
}

/**
  * @brief  Returns sentence type for a string
  * @param  String
  * @retval Sentence ID
  */
sentence_id ICACHE_FLASH_ATTR
get_sentence(const char *string) {
	if (os_strncmp(string + 3, "DAT", 3) == 0) {
		return SENTENCE_DAT;
	} else {
		return SENTENCE_UNKNOWN;
	}
}


/**
  * @brief  Returns talker type for a string
  * @param  String
  * @retval Talker ID
  */
talker_id ICACHE_FLASH_ATTR
get_talker(const char *string) {
	if (os_strncmp(string + 1, "SE", 2) == 0) {
		return TALKER_SEAD;
	} else {
		return TALKER_UNKNOWN;
	}
}

/**
  * @brief  Populates fields from string
  * @param  String, fields
  * @retval Success or failure
  */
bool ICACHE_FLASH_ATTR
process_message(char *string, uint16_t *wattage, uint32_t *timestamp) {
	char *buff_ptr = NULL;
	bool return_value = true;
	//check preamble for talker type, and message type
	talker_id talker = get_talker(string);
	sentence_id sentence = get_sentence(string);
	if (talker == TALKER_SEAD) {
		//uart0_sendStr("talker sead\r\n");
		switch (sentence) {
		case SENTENCE_DAT:
			//process data fields
			//add 7 to the pointer to skip talker id, and message type
			buff_ptr = string + 7;
			//converts string wattage found to an integer, not a float
			*wattage = atoi(buff_ptr);
			while (*buff_ptr != ',') {
				buff_ptr++;
			}
			buff_ptr++;
			//copies timestamp to send buffer timestamp
			uint8_t i = 0;
			*timestamp = stringtoint(buff_ptr);
			break;
		case SENTENCE_UNKNOWN:
			DEBUG_PRINT(("sentence unknown\r\n"));
			return_value = false;
			break;
		default:
			return_value = false;
			break;
		}
	} else if (talker == TALKER_UNKNOWN) {
		DEBUG_PRINT(("talker unknown\r\n"));
		return_value = false;
	}
	//os_printf("time: %s\r\n", timestamp);
   if (*wattage > 2000) {
      DEBUG_PRINT(("string was %s\r\n", string));
   }
	//os_printf("watt: %d\r\n", *wattage);
	return return_value;
}
