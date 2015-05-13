/*
 * Henry Crute
 * hcrute@ucsc.edu
 * 	
 * Extra library functions for string to int and int to string
 * conversions
 * 
 */

#include "extralib.h"
#include "c_types.h"
#include "espmissingincludes.h"
#include "osapi.h"

/**
  * @brief  Turns decimal string into a 64 bit value
  * @param  String
  * @retval Talker ID
  */
uint64_t ICACHE_FLASH_ATTR
stringtoint(char *p) {
	uint64_t temp = 0;
	while (*p  > 47 && *p < 58) {
		temp = (temp << 3) + (temp << 1) + (*p) - '0';
      p++;
	}
	return temp;
}

/**
  * @brief  Turns decimal number into hexadecimal string
  * @param  String
  * @retval Talker ID
  */
char* ICACHE_FLASH_ATTR
inttohexstring(uint64_t i, char *b){
	unsigned int lower = i & 0x00000000FFFFFFFF;
	unsigned int upper = (uint64_t)i >> 32;
	char *ptr = b;
   os_sprintf(ptr, "%x%08x", upper, lower);
   return b;
}
