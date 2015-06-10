#!/bin/bash

GITDIR=$(pwd)

sudo apt-get update -y

sudo apt-get install nginx python2.7-dev python-flup python-pip git -y

sudo apt-get install binutils libproj-dev gdal-bin -y

sudo apt-get build-dep python-matplotlib -y

sudo apt-get install mysql-server python-mysqldb -y


echo "Creating new Django Database User."
echo -n "Django Database Username: "
read username
echo -n "Django Database Password: "
read -s password
echo -n "Django Database Name: "
read db_name

echo -n "Django Root Database Password: \n"

mysql -u root -e "CREATE USER '${username}'@'%' IDENTIFIED BY '${password}'; GRANT ALL PRIVILEGES ON *.* TO '${username}'@'%' WITH GRANT OPTION;" -p

echo -n "Django Root Database Password Again: \n"

mysql -u root -e "CREATE DATABASE ${db_name};" -p

sudo pip install django uwsgi

cd ~

# check if influxdb is installed. If not, download and install

dpkg -s influxdb >/dev/null 2>&1 && {
	echo "influxdb is already installed."
} || {
	wget https://s3.amazonaws.com/influxdb/influxdb_latest_amd64.deb

	sudo dpkg -i influxdb_latest_amd64.deb

	rm influxdb_latest_amd64.deb
        curl -X POST 'http://localhost:8086/db?u=root&p=root' \
           -d '{"name": "seads"}'
}

cd "${GITDIR}"

pip install -r requirements.txt

python "${GITDIR}"/custom_config.py

cp deploy/uwsgi.conf ~

cd ~

sudo mv /etc/nginx/sites-enabled/default ~

sudo ln -s nginx.conf /etc/nginx/sites-enabled

sudo cp uwsgi.conf /etc/init/uwsgi.conf

cd /etc/init.d/

sudo echo "sudo nginx -c /srv/nginx.conf" > /etc/init.d/nginx-passenger.sh

sudo chmod +x /etc/init.d/nginx-passenger.sh

sudo update-rc.d nginx-passenger.sh defaults

cd ~

sudo chown root /srv/uwsgi.ini

sudo chown root /srv/nginx.conf

cd "${GITDIR}"

# Change mysql settings to configure a remote database
sudo python "${GITDIR}"/custom_mysql.py

sudo python "${GITDIR}"/manage.py migrate

sudo python "${GITDIR}"/manage.py migrate

read -p "Create Django superuser? [y/n] " -n 1 -r
echo    
if [[ $REPLY =~ ^[Yy]$ ]]
then
    sudo python "${GITDIR}"/manage.py createsuperuser
fi

sudo chown -R www-data "${GITDIR}"

