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

#include "uart.h"
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
					"Content-Type: application/x-www-form-urlencoded\r\n"\
					"Content-Length: %u\r\n\r\n"\
					"{\"serial\":\"%s\"}"

//the preamble of the post request
#define POST_REQUEST "POST /api/event-api/ HTTP/1.1\r\n"\
					 "User-Agent: ESP8266\r\n"\
					 "Host: seads.brabsmit.com\r\n"\
					 "Accept: */*\r\n"\
					 "Content-Type: application/x-www-form-urlencoded\r\n"\
					 "Content-Length: %u\r\n\r\n%s"

//format string for json data
#define JSON_DATA	"{\"device\":\"/api/device-api/%s/\","\
					"\"dataPoints\":[%s]}\r\n"

#define DATA_POINT	"{\"timestamp\":%s,\"wattage\":%d}"

bool make_device = FALSE;

//approximately in seconds
#define WIFI_CONFIG_TIMEOUT 30

//connection tries
uint8_t connect_try = 0;

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

/**
  * @brief  Prints debug information to uart about the espconn state
  * @param  ESP Connection struct
  * @retval None
  */
void print_espconn_state(espconn *serv_connection) {
	os_printf("espconn state is: %d\r\n", serv_connection->state);
}

/**
  * @brief  Main logic controlling the http connection, and requests
  * @param  Data to send
  * @retval True if succeeded, false if failed to send
  */
bool ICACHE_FLASH_ATTR
send_http_request(send_data_t *temp) {
	bool return_value = false;
	//check to see if we have an IP address and we are in STA mode
	//1 stands for STA mode 2 is AP and 3 is both STA+AP
	if (wifi_station_get_connect_status() == STATION_GOT_IP &&
		wifi_get_opmode() == 1) {
		os_printf("Station has IP address\r\n");
		data_to_send = temp;
		//if the server connection isn't up???? start it
		if (serv_conn.state == ESPCONN_NONE ||
			serv_conn.state == ESPCONN_CLOSE) {
			network_start();
		} else if (serv_conn.state == ESPCONN_CONNECT) {
			sint8 d = package_send(&serv_conn);
		}
		return_value = true;
	} else if (wifi_get_opmode() == 1) {
		//go back into AP mode after 5 unsuccessfull attempts
		if (connect_try++ >= WIFI_CONFIG_TIMEOUT) {
			connect_try = 0;
			os_printf("System resetting for wifi config\r\n");
			wifi_set_opmode(2);
			system_restart();
		} else {
			os_printf("System going into AP mode after %d more attempts\r\n",
					  WIFI_CONFIG_TIMEOUT - connect_try);
		}
		return_value = false;
	} else {
		os_printf("No ip addr and not in STA mode\r\n");
		return_value = false;
	}
	//finished with initiating networking requests
	done_sending();
	return return_value;
}

/**
  * @brief  Formatts and packages data to be sent, and sends it
  * @param  The server connection
  * @retval True if succeeded, false if failed
  */
sint8 ICACHE_FLASH_ATTR
package_send(espconn *serv_conn) {
	//init variables
	char data_points[512] = "";
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
	chars_written = os_sprintf(data_points, DATA_POINT, 
		data_to_send->timestamp, data_to_send->wattage);
	chars_written = os_sprintf(json_data, JSON_DATA, DEVICE_ID,
		data_points);
	chars_written = os_sprintf(send_data, POST_REQUEST, chars_written,
		json_data);
	os_printf("\r\nSend Data:\r\n%s\r\n", send_data);
	//send the data.
	return espconn_sent(serv_conn,(uint8 *)send_data,strlen(send_data));
}

/**
  * @brief  Server connection callback on data sent
  * @param  The esp connection
  * @retval None
  */
static void ICACHE_FLASH_ATTR
networkSentCb(void *arg) {
  os_printf("sent\r\n");
  print_espconn_state((espconn *)arg);
  //incriments circular buffer tail (POP POP)
  pop_pop_buffer();
}

/**
  * @brief  Received callback for server connection
  * @param  The esp connection
  * @retval None
  */
static void ICACHE_FLASH_ATTR
networkRecvCb(void *arg, char *data, unsigned short len) {
	uint8_t i;
	os_printf("recv\r\n");
	print_espconn_state((espconn *)arg);
	//print out what was received
	espconn *serv_conn=(espconn *)arg;
	//print out only the http code returned 
	for (i = 0; i < 12; i++) {
		uart0_putChar(data[i]);
	}
	uart0_putChar('\r');
	uart0_putChar('\n');
	//jsonparse_state json_state;
	//jsonparse_setup(json_state, data, len);
	print_espconn_state(serv_conn);
}

/**
  * @brief  Server connection callback on network initially connected
  * @param  The esp connection
  * @retval None
  */
static void ICACHE_FLASH_ATTR
networkConnectedCb(void *arg) {
	os_printf("conn_start\r\n");
	print_espconn_state((espconn *)arg);
	os_printf("conn_end\r\n");
	print_espconn_state((espconn *)arg);
	//set sending flag to false
}

/**
  * @brief  Server connection callback on disconnect with reconnect
  * @param  The esp connection
  * @retval None
  */
static void ICACHE_FLASH_ATTR
networkReconCb(void *arg, sint8 err) {
	server_config = false;
	os_printf("rcon\r\n");
	espconn_disconnect((espconn *)arg);
	print_espconn_state((espconn *)arg);
}

/**
  * @brief  Server connection callback on disconnect
  * @param  The esp connection
  * @retval None
  */
static void ICACHE_FLASH_ATTR
networkDisconCb(void *arg) {
	server_config = false;
	os_printf("dcon\r\n");
	espconn_disconnect((espconn *)arg);
	print_espconn_state((espconn *)arg);
}

/**
  * @brief  Server connection callback on server connection found
  * @param  The esp connection, server ip, and hostname
  * @retval None
  */
static void ICACHE_FLASH_ATTR
networkServerFoundCb(const char *name, ip_addr_t *serv_ip, void *arg) {
	//initializing connection arguments
	static esp_tcp tcp;
	espconn *serv_conn=(espconn *)arg;
	if (serv_ip==NULL) {
		os_printf("\r\nNS lookup failed\r\n");
		return;
	}
	//the destination IP address from NS lookup
	os_printf("\r\nDST: %d.%d.%d.%d\r\n",
	*((uint8 *)&serv_ip->addr), *((uint8 *)&serv_ip->addr + 1),
	*((uint8 *)&serv_ip->addr + 2), *((uint8 *)&serv_ip->addr + 3));
	//specify the connection to be tcp
	serv_conn->type=ESPCONN_TCP;
	serv_conn->state=ESPCONN_NONE;
	serv_conn->proto.tcp=&tcp;
	//specify port number 80 and ip address
	serv_conn->proto.tcp->local_port=espconn_port();
	serv_conn->proto.tcp->remote_port=80;
	os_memcpy(serv_conn->proto.tcp->remote_ip, &serv_ip->addr, 4);
	//specify callback functions for different situations
	espconn_regist_connectcb(serv_conn, networkConnectedCb);
	espconn_regist_disconcb(serv_conn, networkDisconCb);
	espconn_regist_reconcb(serv_conn, networkReconCb);
	espconn_regist_recvcb(serv_conn, networkRecvCb);
	espconn_regist_sentcb(serv_conn, networkSentCb);
	//debug
	print_espconn_state(serv_conn);
	espconn_connect(serv_conn);
	//debug
	print_espconn_state(serv_conn);
}

/**
  * @brief  Server connection initialization/reinitialization
  * @param  None
  * @retval None
  */
void ICACHE_FLASH_ATTR
network_start(void) {
	os_printf("Looking up server...\r\n");
	espconn_gethostbyname(&serv_conn, SERVER_NAME, &serv_ip,
						  networkServerFoundCb);
	print_espconn_state(&serv_conn);
}
