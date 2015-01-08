#!/bin/bash
#install dependencies, and create working directory
sudo apt-get install git autoconf build-essential gperf bison flex texinfo libtool libncurses5-dev wget gawk libc6-dev-i386 python-serial libexpat-dev
sudo mkdir /opt/Espressif
sudo chown $USER /opt/Espressif/
#xtensa crosstool-NG
cd /opt/Espressif
git clone -b lx106 git://github.com/jcmvbkbc/crosstool-NG.git
cd crosstool-NG
./bootstrap && ./configure --prefix=`pwd` && make && make install
./ct-ng xtensa-lx106-elf
./ct-ng build
PATH=/opt/Espressif/crosstool-NG/builds/xtensa-lx106-elf/bin:$PATH
cd ~
echo "export PATH=/opt/Espressif/crosstool-NG/builds/xtensa-lx106-elf/bin:$PATH" >> .bashrc
source .bashrc
#set up the espressif sdk
cd /opt/Espressif
mkdir ESP8266_SDK
wget -O esp_iot_sdk_v0.9.3_14_11_21.zip https://github.com/esp8266/esp8266-wiki/raw/master/sdk/esp_iot_sdk_v0.9.3_14_11_21.zip
wget -O esp_iot_sdk_v0.9.3_14_11_21_patch1.zip https://github.com/esp8266/esp8266-wiki/raw/master/sdk/esp_iot_sdk_v0.9.3_14_11_21_patch1.zip
unzip esp_iot_sdk_v0.9.3_14_11_21.zip
unzip esp_iot_sdk_v0.9.3_14_11_21_patch1.zip
mv esp_iot_sdk_v0.9.3/* ESP8266_SDK
mv License ESP8266_SDK/
rm -rf esp_iot_sdk_v0.9.3
#patching? its on the toolchain website
cd /opt/Espressif/ESP8266_SDK
sed -i -e 's/xt-ar/xtensa-lx106-elf-ar/' -e 's/xt-xcc/xtensa-lx106-elf-gcc/' -e 's/xt-objcopy/xtensa-lx106-elf-objcopy/' Makefile
mv examples/IoT_Demo .
#install xtensa libraries and headers
cd /opt/Espressif/ESP8266_SDK
wget -O lib/libc.a https://github.com/esp8266/esp8266-wiki/raw/master/libs/libc.a
wget -O lib/libhal.a https://github.com/esp8266/esp8266-wiki/raw/master/libs/libhal.a
wget -O include.tgz https://github.com/esp8266/esp8266-wiki/raw/master/include.tgz
tar -xvzf include.tgz
#install esp image tool
cd /opt/Espressif
wget -O esptool_0.0.2-1_i386.deb https://github.com/esp8266/esp8266-wiki/raw/master/deb/esptool_0.0.2-1_i386.deb
sudo dpkg -i esptool_0.0.2-1_i386.deb
#install esp upload tool
cd /opt/Espressif
git clone https://github.com/themadinventor/esptool esptool-py
sudo ln -s /opt/Espressif/esptool-py/esptool.py /opt/Espressif/crosstool-NG/builds/xtensa-lx106-elf/bin/
#put symlinks for upload tool, and crosstool-ng in /usr/bin
sudo ln -s /opt/Espressif/esptool-py/esptool.py /usr/bin/
sudo ln -s /opt/Espressif/crosstool-NG/builds/xtensa-lx106-elf/bin/xtensa-lx106-elf-gcc /usr/bin/xt-xcc
sudo ln -s /opt/Espressif/crosstool-NG/builds/xtensa-lx106-elf/bin/xtensa-lx106-elf-gcc-ar /usr/bin/xt-ar
sudo ln -s /opt/Espressif/crosstool-NG/builds/xtensa-lx106-elf/bin/xtensa-lx106-elf-nm /usr/bin/xt-nm
sudo ln -s /opt/Espressif/crosstool-NG/builds/xtensa-lx106-elf/bin/xtensa-lx106-elf-cpp /usr/bin/xt-cpp
sudo ln -s /opt/Espressif/crosstool-NG/builds/xtensa-lx106-elf/bin/xtensa-lx106-elf-objcopy /usr/bin/xt-objcopy
sudo ln -s /opt/Espressif/crosstool-NG/builds/xtensa-lx106-elf/bin/xtensa-lx106-elf-readelf /usr/bin/xt-readelf
sudo ln -s /opt/Espressif/crosstool-NG/builds/xtensa-lx106-elf/bin/xtensa-lx106-elf-objdump /usr/bin/xt-objdump
