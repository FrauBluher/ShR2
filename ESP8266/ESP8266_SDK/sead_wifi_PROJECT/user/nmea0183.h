/*
 * Henry Crute
 * hcrute@ucsc.edu
 * 	
 * header file for nmea checksums library on esp8266
 * 
 */

#ifndef __NMEA0183_H
#define __NMEA0183_H

typedef enum {
  TALKER_SEAD,
  TALKER_UNKNOWN
}talker_id;

typedef enum {
	SENTENCE_DAT,
	SENTENCE_UNKNOWN
}sentence_id;

//prototype definitions start for library

uint8_t checksum(const char *);

bool check(const char *, bool);

sentence_id get_sentence(const char *);
talker_id get_talker(const char *);
bool process_message(char *, uint16_t *, char *);

#endif
