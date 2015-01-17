#!/bin/bash -x

cd /opt/Espressif/ESP8266_SDK/at_v0.20_on_SDKv0.9.3/bin
/opt/Espressif/esptool-py/esptool.py --port /dev/ttyUSB0 write_flash 0x00000 eagle.app.v6.flash.bin 0x40000 eagle.app.v6.irom0text.bin

