#!/bin/bash

clear
sudo su
cd release/build/arm926
cp lib* /usr/local/lib
chmod 0755 /usr/local/lib/libftd2xx.so.1.1.12
ln -sf /usr/local/lib/libftd2xx.so.1.1.12 /usr/local/lib/libftd2xx.so
exit
