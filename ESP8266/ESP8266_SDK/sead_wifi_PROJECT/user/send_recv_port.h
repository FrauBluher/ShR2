/*
 * Henry Crute
 * hcrute@ucsc.edu
 * 	
 * header file for sending through wifi, and receiving through uart
 * 
 */

#ifndef __SEND_RECV_PORT_H
#define __SEND_RECV_PORT_H

//for the os_event_t type
#include "user_interface.h"

//# defines for the queue tasking config
#define recv_messagePrio			0
#define recv_messageQueueLen		64

#define store_send_messagePrio		1
#define store_send_messageQueueLen	1

//receive store send states
typedef enum {
	IDLE,
	RECEIVE,
	STORE,
	RECEIVE_STORE
}rss_state;

//prototype definitions start for library

//init the two recv and send messages
void send_recv_init(void);
//receiving messages via uart
void recv_message(os_event_t *events);
//storing messages via uart buffer?
void store_message(os_event_t *events);
//sending messages via wifi
void send_message(os_event_t *events);
//sets the flag of sending to false
void done_sending(void);
//sets the config flag to true
void done_config(void);
//sets the sending rate
void set_rate(uint16_t);

#endif
