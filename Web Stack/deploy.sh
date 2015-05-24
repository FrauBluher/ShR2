#!/bin/bash

GITDIR=`pwd`

sudo apt-get update -y

sudo apt-get install nginx python2.7-dev python-flup python-pip git -y

sudo apt-get install binutils libproj-dev gdal-bin -y

sudo pip install django uwsgi

cd ~

wget https://s3.amazonaws.com/influxdb/influxdb_latest_amd64.deb

sudo dpkg -i influxdb_latest_amd64.deb

rm influxdb_latest_amd64.deb

cd $GITDIR

pip install -r requirements.txt

cp uwsgi.ini uwsgi.conf nginx.conf ~

cd ~

sed -i "s/\/home\/ubuntu\/seads\/Web Stack\/webapp\/static\//${GITDIR}\/Web Stack\/webapp\/static\//g" nginx.conf

sudo mv /etc/nginx/sites-enabled/default ~

sudo ln -s nginx.conf /etc/nginx/sites-enabled

sudo cp uwsgi.conf /etc/init/uwsgi.conf

sudo chown root uwsgi.ini

sudo chown root nginx.conf

sudo python $GITDIR/manage.py migrate

sudo python $GITDIR/manage.py createsuperuser