#!/bin/bash

GITDIR=$(pwd)

sudo apt-get update -y

sudo apt-get install nginx python2.7-dev python-flup python-pip git -y

sudo apt-get install binutils libproj-dev gdal-bin mysql-client python-mysqldb -y

sudo pip install django uwsgi

pip install -r requirements.txt

python custom_config.py

cp deploy/uwsgi.conf /srv/

cd /srv/

sudo mv /etc/nginx/sites-enabled/default /srv/

sudo ln -s nginx.conf /etc/nginx/sites-enabled

sudo cp uwsgi.conf /etc/init/uwsgi.conf

cd /etc/init.d/

sudo echo "sudo nginx -c /srv/nginx.conf" > /etc/init.d/nginx-passenger.sh

sudo chmod +x /etc/init.d/nginx-passenger.sh

sudo update-rc.d nginx-passenger.sh defaults

cd /srv/

sudo chown root uwsgi.ini

sudo chown root nginx.conf
