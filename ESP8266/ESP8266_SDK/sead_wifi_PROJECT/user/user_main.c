

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain 
 * this notice you can do whatever you want with this stuff. If we meet some day, 
 * and you think this stuff is worth it, you can buy me a beer in return. 
 * ----------------------------------------------------------------------------
 * 
 * Modified by 
 * hcrute@ucsc.edu
 * Henry Crute
 * 
 */


#include "espmissingincludes.h"
#include "ets_sys.h"
#include "osapi.h"
#include "httpd.h"
#include "httpdespfs.h"
#include "cgi.h"
#include "cgiwifi.h"
#include "uart.h"

#include "send_recv_port.h"

HttpdBuiltInUrl builtInUrls[]={
	{"/", cgiRedirect, "/index.tpl"},
	{"/index.tpl", cgiEspFsTemplate, tplCounter},

	//Routines to make the /wifi URL and everything beneath it work.
	{"/wifi", cgiRedirect, "/wifi/wifi.tpl"},
	{"/wifi/", cgiRedirect, "/wifi/wifi.tpl"},
	{"/wifi/wifiscan.cgi", cgiWiFiScan, NULL},
	{"/wifi/wifi.tpl", cgiEspFsTemplate, tplWlan},
	{"/wifi/connect.cgi", cgiWiFiConnect, NULL},
	{"/wifi/setmode.cgi", cgiWifiSetMode, NULL},


	{"*", cgiEspFsHook, NULL}, //Catch-all cgi function for the filesystem
	{NULL, NULL, NULL}
};


void ICACHE_FLASH_ATTR
user_init(void) {
	uart_init(BIT_RATE_115200);
	httpdInit(builtInUrls, 80);
	send_recv_init();
	os_printf("\nReady\n");
}
