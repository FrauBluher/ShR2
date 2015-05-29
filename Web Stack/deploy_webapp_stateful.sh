#!/bin/bash

GITDIR=$(pwd)

# Install libraries for mysql server and the python interface
sudo apt-get install mysql-server python-mysqldb -y

# Install django to interface with the database
sudo pip install django

# Install the dependencies that accompany this project
pip install -r requirements.txt

# Prompt for superuser creation. Optional.
read -p "Create Django superuser? [y/n] " -n 1 -r
echo    
if [[ $REPLY =~ ^[Yy]$ ]]
then
    sudo python "${GITDIR}"/manage.py createsuperuser
fi

sudo python "${GITDIR}"/manage.py migrate

# Change mysql settings to configure a remote database
sudo python custom_mysql.py

# Register cron jobs
sudo crontab -l > sudocron
echo "@weekly         python ${GITDIR}/manage.py dbbackup >>'${GITDIR}/webapp/dbbackup.log' 2>&1" >> sudocron
echo "@daily          python ${GITDIR}/manage.py email_interval >>'${GITDIR}/webapp/email.log' 2>&1" >> sudocron
echo "@daily          python ${GITDIR}/manage.py check_glacier_jobs >>'${GITDIR}/webapp/glacier.log' 2>&1" >> sudocron
echo "@monthly        python ${GITDIR}/manage.py reset_kilowatt_accumulations monthly" >> sudocron
echo "@daily          python ${GITDIR}/manage.py reset_kilowatt_accumulations daily" >> sudocron
sudo crontab sudocron
rm sudocron