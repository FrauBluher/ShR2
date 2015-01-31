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

#include "buffers.h"
#include "send_recv_port.h"

#define SERVER_NAME "seads.brabsmit.com"

//test string to send to server
#define GET_ECHO 	"GET /echo/ HTTP/1.1\r\n"\
					"User-Agent: ESP8266\r\n"\
					"Host: seads.brabsmit.com\r\n"\
					"Accept: */*\r\n\r\n"

//"Authorization: Token 0d1e0f4b56e4772fdb440abf66da8e2c1df799c0\r\n"

//the preamble of the post request
#define POST_REQUEST "POST /api/event-api/ HTTP/1.1\r\n"\
					 "User-Agent: ESP8266\r\n"\
					 "Host: seads.brabsmit.com\r\n"\
					 "Accept: */*\r\n"\
					 "Content-Type: application/json\r\n"\
					 "Content-Length: %u\r\n\r\n%s"
//format string for json data
#define JSON_DATA "{\"device\": \"/api/device-api/%u/\", \"wattage\":\"%d\", \"timestamp\":\"%s\"}"

uint8_t device_id = 3;

//len 81
//AT+CIPSEND=81

//prototypes
static void networkSentCb(void *);
static void networkRecvCb(void *, char *, unsigned short);
static void networkConnectedCb(void *);
//static void networkReconCb(void *, sint8);
static void networkDisconCb(void *);

void network_start(void);

send_data_t *data_to_send = NULL;
	
bool ICACHE_FLASH_ATTR
send_http_request (send_data_t *temp) {
	//check to see if we have an IP address and we are in STA mode
	//1 stands for STA mode 2 is AP and 3 is both STA+AP
	if (wifi_station_get_connect_status() == STATION_GOT_IP &&
		wifi_get_opmode() == 1) {
		os_printf("Connected and have an ip addr\r\n");
		data_to_send = temp;
		network_start();
		return true;
	} else {
		os_printf("No ip addr\r\n");
		return false;
	}
}


static void ICACHE_FLASH_ATTR networkSentCb(void *arg) {
  os_printf("sent\r\n");
  pop_pop_buffer();
}
 
static void ICACHE_FLASH_ATTR networkRecvCb(void *arg, char *data, unsigned short len) {
	os_printf("recv\r\n");
	//print out what was received
	struct espconn *serv_conn=(struct espconn *)arg;
	int x;
	os_printf("%s\r\n", data);
	espconn_disconnect(serv_conn);
}
 
static void ICACHE_FLASH_ATTR networkConnectedCb(void *arg) {
	os_printf("conn_start\r\n");
	struct espconn *serv_conn=(struct espconn *)arg;
	//sends the echo thingy
	char json_data[512] = "";
	char send_data[1024] = "";
	uint16_t chars_written = 0;
	chars_written = os_sprintf(json_data, JSON_DATA, device_id,
		data_to_send->wattage,
		data_to_send->timestamp);
	chars_written = os_sprintf(send_data, POST_REQUEST, chars_written,
		json_data);
	//send the "send_data" and close the tcp connection afterwards
	os_printf("\r\nSend Data:\r\n%s", send_data);
	
	sint8 d = espconn_sent(serv_conn,(uint8 *)send_data,strlen(send_data));
	/*
	int head_len = strlen(header);
	int json_len = strlen(json);
 
	char json_len_str[10];
	os_sprintf(json_len_str,"%d",json_len);
	int json_len_str_len = strlen(json_len_str);
 
	strcpy(transmission,header);
	strcpy(transmission+head_len,json_len_str);
	strcpy(transmission+head_len+json_len_str_len,"\r\n\r\n");
	strcpy(transmission+head_len+json_len_str_len+4,json);
 
	sint8 d = espconn_sent(serv_conn,transmission,strlen(transmission));
	*/
	espconn_regist_recvcb(serv_conn, networkRecvCb);
	os_printf("conn_end\r\n");
	//sets sending flag false
}

//TODO handle reconnection.
/*
static void ICACHE_FLASH_ATTR networkReconCb(void *arg, sint8 err) {
	os_printf("rcon");
	struct espconn *serv_conn=(struct espconn *)arg;
	espconn_disconnect(serv_conn);
}*/

static void ICACHE_FLASH_ATTR networkDisconCb(void *arg) {
	os_printf("dcon");
	done_sending();
}

static void ICACHE_FLASH_ATTR
networkServerFoundCb(const char *name, ip_addr_t *serv_ip, void *arg) {
	//initializing connection arguments
	static esp_tcp tcp;
	struct espconn *serv_conn=(struct espconn *)arg;
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
	serv_conn->proto.tcp->remote_port=80;
	os_memcpy(serv_conn->proto.tcp->remote_ip, &serv_ip->addr, 4);
	//specify callback functions for different situations
	espconn_regist_connectcb(serv_conn, networkConnectedCb);
	espconn_regist_disconcb(serv_conn, networkDisconCb);
	//espconn_regist_reconcb(serv_conn, networkReconCb);
	espconn_regist_recvcb(serv_conn, networkRecvCb);
	espconn_regist_sentcb(serv_conn, networkSentCb);
	espconn_connect(serv_conn);
}

void ICACHE_FLASH_ATTR
network_start(void) {
	//initializing connection arguments
	static struct espconn serv_conn;
	static ip_addr_t serv_ip;
	os_printf("Looking up server...\r\n");
	espconn_gethostbyname(&serv_conn, SERVER_NAME, &serv_ip,
						  networkServerFoundCb);
}
