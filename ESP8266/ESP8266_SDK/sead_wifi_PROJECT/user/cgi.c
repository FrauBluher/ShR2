/*
Some random cgi routines. Used in the LED example and the page that returns the entire
flash as a binary. Also handles the hit counter on the main page.
*/

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain 
 * this notice you can do whatever you want with this stuff. If we meet some day, 
 * and you think this stuff is worth it, you can buy me a beer in return. 
 * ----------------------------------------------------------------------------
 */


#include <string.h>
#include <osapi.h>
#include "user_interface.h"
#include "mem.h"
#include "httpd.h"
#include "cgi.h"
#include <ip_addr.h>
#include "espmissingincludes.h"

static long hitCounter=0;

void ICACHE_FLASH_ATTR tplSetupPage(HttpdConnData *connData, char *token, void **arg) {
	//Taken from tplWlan in cgiwifi.c
	char buff[1024];
	int x;
	static struct station_config stconf;
	if (token==NULL) return;
	wifi_station_get_config(&stconf);

	os_strcpy(buff, "Unknown");
	if (os_strcmp(token, "WiFiMode")==0) {
		x=wifi_get_opmode();
		if (x==1) os_strcpy(buff, "Client");
		if (x==2) os_strcpy(buff, "SoftAP");
		if (x==3) os_strcpy(buff, "STA+AP");
	} else if (os_strcmp(token, "currSsid")==0) {
		os_strcpy(buff, (char*)stconf.ssid);
	} else if (os_strcmp(token, "WiFiPasswd")==0) {
		os_strcpy(buff, (char*)stconf.password);
	} else if (os_strcmp(token, "WiFiapwarn")==0) {
		x=wifi_get_opmode();
		if (x==2) {
			os_strcpy(buff, "<b>Can't scan in this mode.</b> Click <a href=\"/wifi/setmode.cgi?mode=3\">here</a> to go to STA+AP mode.");
		} else {
			os_strcpy(buff, "Click <a href=\"/wifi/setmode.cgi?mode=2\">here</a> to go to standalone AP mode.");
		}
	//Taken from tplCounter below
	} else if (os_strcmp(token, "counter")==0) {
		hitCounter++;
		os_sprintf(buff, "%ld", hitCounter);
	}
	httpdSend(connData, buff, -1);
}

/*
//Template code for the counter on the index page.
void ICACHE_FLASH_ATTR tplCounter(HttpdConnData *connData, char *token, void **arg) {
	char buff[128];
	if (token==NULL) return;

	if (os_strcmp(token, "counter")==0) {
		hitCounter++;
		os_sprintf(buff, "%ld", hitCounter);
	}
	httpdSend(connData, buff, -1);
}
*/
