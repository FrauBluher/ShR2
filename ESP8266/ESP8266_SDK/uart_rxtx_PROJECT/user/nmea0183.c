/*
 *  Henry Crute
 * 	hcrute@ucsc.edu
 * 	
 * 	code for nmea protocol
 * 
 *  
 * 
 * 
 * 
 */

#include "c_types.h"

/**
  * @brief  nmea string checksum function
  * @param  s input string to check
  * @retval c output XOR of all the bytes in the sentence, not including
  * the initial dollar sign
  */

uint8_t ICACHE_FLASH_ATTR
checksum(char *s) {
	uint8_t c = 0;
	
	while(*s) {
		c ^= *s++;
	}
	return c;
}
