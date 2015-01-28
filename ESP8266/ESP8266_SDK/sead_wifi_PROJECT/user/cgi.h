#ifndef CGI_H
#define CGI_H

#include "httpd.h"

int cgiReadFlash(HttpdConnData *connData);
void tplCounter(HttpdConnData *connData, char *token, void **arg);
void tplSetupPage(HttpdConnData *connData, char *token, void **arg);

#endif
