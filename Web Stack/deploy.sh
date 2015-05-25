#!/bin/bash

GITDIR=$(pwd)

sudo apt-get update -y

sudo apt-get install nginx python2.7-dev python-flup python-pip git -y

sudo apt-get install binutils libproj-dev gdal-bin -y

sudo pip install django uwsgi

cd ~

# check if influxdb is installed. If not, download and install

dpkg -s influxdb >/dev/null 2>&1 && {
	echo "influxdb is already installed."
} || {
	wget https://s3.amazonaws.com/influxdb/influxdb_latest_amd64.deb

	sudo dpkg -i influxdb_latest_amd64.deb

	rm influxdb_latest_amd64.deb
}

cd "${GITDIR}"

pip install -r requirements.txt

python custom_config.py

cd ~

sudo mv /etc/nginx/sites-enabled/default ~

sudo ln -s nginx.conf /etc/nginx/sites-enabled

sudo cp uwsgi.conf /etc/init/uwsgi.conf

cd /etc/init.d/

sudo echo "sudo nginx -c /home/ubuntu/nginx.conf" > /etc/init.d/nginx-passenger.sh

sudo chmod +x /etc/init.d/nginx-passenger.sh

sudo update-rc.d nginx-passenger.sh defaults

cd ~

sudo chown root uwsgi.ini

sudo chown root nginx.conf

sudo python "${GITDIR}"/manage.py migrate

read -p "Create Django superuser? [y/n] " -n 1 -r
echo    
if [[ $REPLY =~ ^[Yy]$ ]]
then
    sudo python "${GITDIR}"/manage.py createsuperuser
fi

sudo chown -R www-data "${GITDIR}"
