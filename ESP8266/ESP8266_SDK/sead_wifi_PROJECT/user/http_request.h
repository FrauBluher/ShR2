/*
 * Henry Crute
 * hcrute@ucsc.edu
 * 	
 * Implementation of tcp connection, and sending data with a request
 * 
 */


#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "buffers.h"

bool get_http_config(void);

bool send_http_request(circular_send_buffer_t *);

#endif
