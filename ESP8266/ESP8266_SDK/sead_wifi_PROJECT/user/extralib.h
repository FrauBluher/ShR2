/*
 * Henry Crute
 * hcrute@ucsc.edu
 * 	
 * Extra library functions for string to int and int to string
 * conversions
 * 
 */

#ifndef __EXTRALIB_H
#define __EXTRALIB_H

#include "c_types.h"

uint64_t stringtoint(char *);

char *inttohexstring(uint64_t, char *);

#endif
