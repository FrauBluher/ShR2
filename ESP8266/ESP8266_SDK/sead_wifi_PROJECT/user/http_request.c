/*
 *  Henry Crute
 * 	hcrute@ucsc.edu
 * 	
 * Implementation of tcp connection, and sending data with a request
 * 
 */

#include "c_types.h"
#include "espmissingincludes.h"
#include "user_interface.h"
#include "espconn.h"
#include "ssl/cert.h"
#include "ssl/private_key.h"

#include "buffers.h"
#include "send_recv_port.h"

//server name and device ID number
#define SERVER_NAME "seads.brabsmit.com"
#define DEVICE_ID	"3"

//test string to send to server
#define GET_ECHO 	"GET /echo/ HTTP/1.1\r\n"\
					"User-Agent: ESP8266\r\n"\
					"Host: seads.brabsmit.com\r\n"\
					"Accept: */*\r\n\r\n"

//"Authorization: Token 0d1e0f4b56e4772fdb440abf66da8e2c1df799c0\r\n"

// http request to create the device
#define POST_DEVICE	"POST /api/device-api/ HTTP/1.1\r\n"\
					"User-Agent: ESP8266\r\n"\
					"Host: seads.brabsmit.com\r\n"\
					"Accept: */*\r\n"\
					"Content-Type: application/json\r\n"\
					"Content-Length: %u\r\n\r\n"\
					"{\"serial\":\"%s\"}"

//the preamble of the post request
#define POST_REQUEST "POST /api/event-api/ HTTP/1.1\r\n"\
					 "User-Agent: ESP8266\r\n"\
					 "Host: seads.brabsmit.com\r\n"\
					 "Accept: */*\r\n"\
					 "Content-Type: application/json\r\n"\
					 "Content-Length: %u\r\n\r\n%s"
//format string for json data
#define JSON_DATA	"{\"device\": \"/api/device-api/%s/\", \"wattage\":\"%d\", \"timestamp\":\"%s\"}"

bool make_device = FALSE;

//len 81
//AT+CIPSEND=81

//prototypes
static void networkSentCb(void *);
static void networkRecvCb(void *, char *, unsigned short);
static void networkConnectedCb(void *);
static void networkReconCb(void *, sint8);
static void networkDisconCb(void *);
void network_start(void);
sint8 package_send(espconn *);

//initializing connection variables!
static espconn serv_conn;
static ip_addr_t serv_ip;

//pointer to the data we want to send:
//TODO: localize this pointer in function calls!
send_data_t *data_to_send = NULL;
//flag to tell if we have already configured tcp connection to the server.
bool server_config = false;


void print_espconn_state(espconn *serv_connection) {
	os_printf("espconn state is: %d\r\n", serv_connection->state);
}

//occasionally gets stuck in sending phase.
bool ICACHE_FLASH_ATTR
send_http_request(send_data_t *temp) {
	//check to see if we have an IP address and we are in STA mode
	//1 stands for STA mode 2 is AP and 3 is both STA+AP
	if (wifi_station_get_connect_status() == STATION_GOT_IP &&
		wifi_get_opmode() == 1) {
		os_printf("Connected and have an ip addr\r\n");
		data_to_send = temp;
		//if the server connection isn't up???? start it
		if (serv_conn.state == ESPCONN_NONE ||
			serv_conn.state == ESPCONN_CLOSE) {
			network_start();
		} else {
			sint8 d = package_send(&serv_conn);
			done_sending();
		}
		return true;
	} else {
		os_printf("No ip addr\r\n");
		return false;
	}
}

//formates the data from data_to_send to json
//sends it on the serv_conn connection
sint8 ICACHE_FLASH_ATTR
package_send(espconn *serv_conn) {
	//init variables
	char json_data[512] = "";
	char send_data[1024] = "";
	uint16_t chars_written = 0;
	//create device if we haven't done it yet
	if (!make_device) {
		chars_written = os_sprintf(send_data, POST_DEVICE,
			13 + strlen(DEVICE_ID), DEVICE_ID);
		os_printf("\r\nSend Data:\r\n%s", send_data);
		espconn_sent(serv_conn,(uint8 *)send_data,strlen(send_data));
		make_device = TRUE;
	}
	//send regular data
	chars_written = os_sprintf(json_data, JSON_DATA, DEVICE_ID,
		data_to_send->wattage, data_to_send->timestamp);
	chars_written = os_sprintf(send_data, POST_REQUEST, chars_written,
		json_data);
	os_printf("\r\nSend Data:\r\n%s", send_data);
	//send the data.
	return espconn_secure_sent(serv_conn,(uint8 *)send_data,strlen(send_data));
}

static void ICACHE_FLASH_ATTR
networkSentCb(void *arg) {
  os_printf("sent\r\n");
  print_espconn_state((espconn *)arg);
  //incriments circular buffer tail (POP POP)
  pop_pop_buffer();
}
 
static void ICACHE_FLASH_ATTR
networkRecvCb(void *arg, char *data, unsigned short len) {
	os_printf("recv\r\n");
	print_espconn_state((espconn *)arg);
	//print out what was received
	espconn *serv_conn=(espconn *)arg;
	os_printf("%s\r\n", data);
	print_espconn_state(serv_conn);
}

//callback for being initially connected
static void ICACHE_FLASH_ATTR
networkConnectedCb(void *arg) {
	os_printf("conn_start\r\n");
	print_espconn_state((espconn *)arg);
	sint8 d = package_send((espconn *)arg);
	os_printf("conn_end\r\n");
	print_espconn_state((espconn *)arg);
	//set sending flag to false
	done_sending();
}

//TODO handle reconnection?
static void ICACHE_FLASH_ATTR
networkReconCb(void *arg, sint8 err) {
	server_config = false;
	os_printf("rcon");
	print_espconn_state((espconn *)arg);
}

static void ICACHE_FLASH_ATTR
networkDisconCb(void *arg) {
	server_config = false;
	os_printf("dcon");
	print_espconn_state((espconn *)arg);
}

static void ICACHE_FLASH_ATTR
networkServerFoundCb(const char *name, ip_addr_t *serv_ip, void *arg) {
	//initializing connection arguments
	static esp_tcp tcp;
	espconn *serv_conn=(espconn *)arg;
	if (serv_ip==NULL) {
		os_printf("\r\nNS lookup failed :/\r\n");
		return;
	}
	
	os_printf("\r\nDST: %d.%d.%d.%d\r\n",
	*((uint8 *)&serv_ip->addr), *((uint8 *)&serv_ip->addr + 1),
	*((uint8 *)&serv_ip->addr + 2), *((uint8 *)&serv_ip->addr + 3));
	
	//specify the connection to be tcp
	serv_conn->type=ESPCONN_TCP;
	serv_conn->state=ESPCONN_NONE;
	serv_conn->proto.tcp=&tcp;
	//specify port number 80 and ip address
	serv_conn->proto.tcp->local_port=espconn_port();
	serv_conn->proto.tcp->remote_port=443;
	os_memcpy(serv_conn->proto.tcp->remote_ip, &serv_ip->addr, 4);
	//specify callback functions for different situations
	espconn_regist_connectcb(serv_conn, networkConnectedCb);
	espconn_regist_disconcb(serv_conn, networkDisconCb);
	espconn_regist_reconcb(serv_conn, networkReconCb);
	espconn_regist_recvcb(serv_conn, networkRecvCb);
	espconn_regist_sentcb(serv_conn, networkSentCb);
	//debug 
	print_espconn_state(serv_conn);
	espconn_secure_connect(serv_conn);
	//debug
	print_espconn_state(serv_conn);
}

void ICACHE_FLASH_ATTR
network_start(void) {
	os_printf("Looking up server...\r\n");
	espconn_gethostbyname(&serv_conn, SERVER_NAME, &serv_ip,
						  networkServerFoundCb);
	print_espconn_state(&serv_conn);
}
