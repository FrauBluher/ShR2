void ntpSync()
{
  const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
  byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
  int pos;
  unsigned long timeout;
  time_t utc, local;
  TimeChangeRule *tcr;

  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  //Serial.println("2");
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  altSerial.println("AT+CIPSTART=1,\"UDP\",\"192.168.10.253\",123");
  if (!waitforLine("OK", 2000)) {
    Serial.println("Unable to connect to NTP server");
    return;
  }
  altSerial.println("AT+CIPSEND=1,48");
  if (!waitforString(">", 1000)) {
    Serial.println("Prompt missed");
    return;
  }

  Serial.println("Sending query");
  altSerial.write(packetBuffer, NTP_PACKET_SIZE);
  if (!waitforString("+IPD,1,48:", 2000)) {
    Serial.println("NTP Server did not respond");
    return;
  }

  pos = 0;
  timeout = millis() + 1000;
  memset(packetBuffer, 0, NTP_PACKET_SIZE);

  while ((pos < NTP_PACKET_SIZE) && (millis() < timeout)) {
    if (altSerial.available() > 0)
      packetBuffer[pos++] = altSerial.read();
  }

  if (!waitforLine("SEND OK", 2000)) {
    Serial.println("Failed to send?");
  }

  altSerial.println("AT+CIPCLOSE=1");
  if (!waitforLine("OK", 2000)) {
    Serial.println("Unable to close port");
    return;
  }  

  // Serial.print("Bytes received: ");
  // Serial.println(pos);

  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;
  // Serial.print("Seconds since Jan 1 1900 = " );
  // Serial.println(secsSince1900);

  // now convert NTP time into everyday time:
  // Serial.print("Unix time = ");
  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  const unsigned long seventyYears = 2208988800UL;
  // subtract seventy years:
  unsigned long epoch = secsSince1900 - seventyYears;
  // print Unix time:
  // Serial.println(epoch);
  setTime(epoch);

  utc = now();
  printTime(utc, "UTC");
  local = myTZ.toLocal(utc, &tcr);
  printTime(local, tcr -> abbrev);
}

//Function to print time with time zone
void printTime(time_t t, char *tz)
{
    sPrintI00(hour(t));
    sPrintDigits(minute(t));
    sPrintDigits(second(t));
    Serial.print(' ');
    Serial.print(dayShortStr(weekday(t)));
    Serial.print(' ');
    sPrintI00(day(t));
    Serial.print(' ');
    Serial.print(monthShortStr(month(t)));
    Serial.print(' ');
    Serial.print(year(t));
    Serial.print(' ');
    Serial.print(tz);
    Serial.println();
}

//Print an integer in "00" format (with leading zero).
//Input value assumed to be between 0 and 99.
void sPrintI00(int val)
{
    if (val < 10) Serial.print('0');
    Serial.print(val, DEC);
    return;
}

//Print an integer in ":00" format (with leading zero).
//Input value assumed to be between 0 and 99.
void sPrintDigits(int val)
{
    Serial.print(':');
    if(val < 10) Serial.print('0');
    Serial.print(val, DEC);
}

int readline(int readch, char *buffer, int len)
{
  static int pos = 0;
  int rpos;

  if (readch > 0) {
    switch (readch) {
      case '\n':  // Ignore new lines
        break;
      case '\r': // Return on CR
        rpos = pos;
        pos = 0;  // Reset position for next input
        return rpos;
      default:
        if (pos < len-1) {
          buffer[pos++] = readch;
          buffer[pos] = '\0';
        }
    }
  }
  // No end of line found so return -1.
  return -1;
} 

int waitforLine(char *string, int timeout)
{
  unsigned long endtime = millis() + timeout;
  static char buffer[MAXBUF];  

  while (millis() < endtime) {
    checkStatus();
    if (readline(altSerial.read(), buffer, MAXBUF) > 0) {
      // Serial.println(buffer);
      processMessage(buffer);
      if (strstr(buffer, string) != NULL)
        return true;
      if (strstr(buffer, "no change") != NULL)
        return true;
    }
  }
  return false;
}
