/* 
 * File:   nmea0183.h
 * Author: henry
 *
 * Created on February 26, 2015, 12:18 PM
 */

#ifndef NMEA0183_H
#define	NMEA0183_H

#include <stdarg.h>
#include <stdbool.h>
#include <plib.h>
#include <stdint.h>

//sead talker
#define TALKER "SE"
//data message
#define MESSAGE_TYPE "DAT"

char *create_message(char *talker, char *message_type, uint8_t arg_num, ...);
bool destroy_message(char *message);

uint8_t checksum(const char *string);
bool check(const char *string, bool strict);

#endif	/* NMEA0183_H */
