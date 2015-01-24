#!/bin/bash

clear
cd ./release/build/arm926
sudo cp lib* /usr/local/lib
sudo chmod 0755 /usr/local/lib/libftd2xx.so.1.1.12
sudo ln -sf /usr/local/lib/libftd2xx.so.1.1.12 /usr/local/lib/libftd2xx.so

