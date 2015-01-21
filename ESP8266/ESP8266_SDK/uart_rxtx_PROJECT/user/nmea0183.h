/*
 *  Henry Crute
 * 	hcrute@ucsc.edu
 * 	
 * 	header file for nmea checksums library on esp8266
 * 
 */

#ifndef __NMEA0183_H
#define __NMEA0183_H

//prototype definitions start for library

uint8_t checksum(const char *);

bool check(const char *, bool);

#endif
