#!/bin/bash

# check if influxdb is installed. If not, download and install

dpkg -s influxdb >/dev/null 2>&1 && {
        echo "influxdb is already installed."
} || {
        wget https://s3.amazonaws.com/influxdb/influxdb_latest_amd64.deb

        sudo dpkg -i influxdb_latest_amd64.deb

        rm influxdb_latest_amd64.deb
}
