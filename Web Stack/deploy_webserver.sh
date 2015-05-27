#!/bin/bash

GITDIR=$(pwd)

sudo apt-get update -y

sudo apt-get install nginx python2.7-dev python-flup python-pip git -y

sudo apt-get install binutils libproj-dev gdal-bin -y

sudo pip install django uwsgi

pip install -r requirements.txt

python custom_config.py

cp deploy/uwsgi.conf ~

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


sudo crontab -l > sudocron
echo "@weekly         python /home/ubuntu/seads-git/ShR2/Web\ Stack/manage.py dbbackup >>${GITDIR}/webapp/dbbackup.log 2>&1" >> sudocron
echo "@daily          python /home/ubuntu/seads-git/ShR2/Web\ Stack/manage.py email_interval >>${GITDIR}/webapp/email.log 2>&1" >> sudocron
echo "@daily          python /home/ubuntu/seads-git/ShR2/Web\ Stack/manage.py check_glacier_jobs >>${GITDIR}/webapp/glacier.log 2>&1" >> sudocron
echo "@monthly        python /home/ubuntu/seads-git/ShR2/Web\ Stack/manage.py reset_kilowatt_accumulations monthly" >> sudocron
echo "@daily          python /home/ubuntu/seads-git/ShR2/Web\ Stack/manage.py reset_kilowatt_accumulations daily" >> sudocron
echo "@weekly         service uwsgi restart" >> sudocron
sudo crontab sudocron
rm sudocron

