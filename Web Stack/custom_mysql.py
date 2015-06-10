import shutil
import getpass

def main():
   db_name = raw_input('Django database name (django_db): ')
   if db_name == '': db_name = 'django_db'
   db_user = raw_input('Django database username (django): ')
   if db_user == '': db_user = 'django'
   db_host = raw_input('Django database host address (django.seads.io): ')
   if db_host == '': db_host = 'django.seads.io'
   db_port = raw_input('Django database port (3306): ')
   if db_port == '': db_port = '3306'
   db_pswd = getpass.getpass(db_user + " password: ")
   db_root_pswd = getpass.getpass("Django database root password: ")
   influxdb = raw_input('Influxdb address (db.seads.io): ')
   if influxdb == '': influxdb = 'db.seads.io'
   base_url = raw_input('Base address (seads.io): ')
   if base_url == '': base_url = 'seads.io'

   findlines = [
      'port    = 3306',
      'bind-address     = 127.0.0.1',
   ]
   replacelines = [
      'port    = '+str(db_port),
      '#bind-address    = 127.0.0.1',
   ]
   find_replace = dict(zip(findlines, replacelines))

   with open('/etc/mysql/my.cnf') as data:
       with open('my.cnf', 'w') as new_data:
           for line in data:
               for key in find_replace:
                   if key in line:
                       line = line.replace(key, find_replace[key])
               new_data.write(line)
   shutil.move("my.cnf","/etc/mysql/my.cnf")


   # create a dict of find keys and replace values
   findlines = [
      'INFLUXDB_URI = \'db.seads.io\'',
      'BASE_URL = \'seads.io\'',
      '        \'NAME\': \'django_db\',',
      '        \'USER\': \'django\',',
      '        \'PASSWORD\': \'teammantey\',',
      '        \'HOST\': \'django.seads.io\',',
      '        \'PORT\': \'3306\',',
   ]
   replacelines = [
      'INFLUXDB_URI = \''+influxdb+'\'',
      'BASE_URL = \''+base_url+'\'',
      '        \'NAME\': \''+db_name+'\',',
      '        \'USER\': \''+db_user+'\',',
      '        \'PASSWORD\': \''+db_pswd+'\',',
      '        \'HOST\': \''+db_host+'\',',
      '        \'PORT\': \''+str(db_port)+'\',',
   ]
   find_replace = dict(zip(findlines, replacelines))

   with open('seads/settings.py') as data:
       with open('settings.py', 'w') as new_data:
           for line in data:
               for key in find_replace:
                   if key in line:
                       line = line.replace(key, find_replace[key])
               new_data.write(line)
   shutil.move("settings.py","seads/settings.py")

   choice = raw_input("Configure database as server? [y/n]: ").lower()
   if 'y' in choice:
      import mysql.connector
      cnx = mysql.connector.connect(user='root', password=db_root_pswd, host='localhost', db=dn_name)
      cursor = cnx.cursor()
      cursor.execute("GRANT ALL ON "+db_name+".* TO "+db_user+"@'%' IDENTIFIED BY '"+db_pswd+"';")

if __name__ == "__main__":
   main()
