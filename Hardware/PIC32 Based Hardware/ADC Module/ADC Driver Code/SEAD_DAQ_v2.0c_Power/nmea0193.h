/* 
 * File:   nmea0193.h
 * Author: henry
 *
 * Created on February 26, 2015, 12:18 PM
 */

#ifndef NMEA0193_H
#define	NMEA0193_H

char *create_message(char *, char *, uint32_t, uint32_t);
bool destroy_message(char *);

uint8_t checksum(const char *);
bool check(const char *, bool);

#endif	/* NMEA0193_H */
