import shutil
import getpass

def main():
   db_name = raw_input('Django database name (django_db): ')
   if db_name == '': db_name = 'django_db'
   db_user = raw_input('Django database username (django): ')
   if db_user == '': db_user = 'django'
   db_host = raw_input('Django database host address (db.seads.io): ')
   if db_host == '': db_host = 'db.seads.io'
   db_port = raw_input('Django database port (3306): ')
   if db_port == '': db_port = '3306'
   db_pswd = getpass.getpass("Django database password: ")
   influxdb = raw_input('Influxdb address (db.seads.io): ')
   if influxdb == '': influxdb = 'db.seads.io'
   base_url = raw_input('Base address (seads.io): ')
   if base_url == '': base_url = 'seads.io'
   
   with open("my.cnf", "w") as newfile:
      with open("/etc/mysql/my.cnf", "r") as oldfile:
         for line in oldfile:
            newline = line.replace('port		= 3306','port		= '+str(db_port))
            newfile.write(newline)
   with open("my.cnf", "a") as newfile:
      with open("/etc/mysql/my.cnf", "r") as oldfile:
         for line in oldfile:
            newline = line.replace('bind-address		= 127.0.0.1','#bind-address		= 127.0.0.1')
            newfile.write(newline)
   shutil.move("my.cnf","/etc/mysql/my.cnf")
   
   with open("settings.py", "w") as newfile:
      with open("seads/settings.py", "r") as oldfile:
         for line in oldfile:
            newline = line.replace('INFLUXDB_URI = \'db.seads.io\'','INFLUXDB_URI = \''+influxdb+'\'')
            newfile.write(newline)
   with open("settings.py", "a") as newfile:
      with open("seads/settings.py", "r") as oldfile:
         for line in oldfile:
            newline = line.replace('BASE_URL = \'seads.io\'','BASE_URL = \''+base_url+'\'')
            newfile.write(newline)
   with open("settings.py", "a") as newfile:
      with open("seads/settings.py", "r") as oldfile:
         for line in oldfile:
            newline = line.replace('        \'NAME\': \'django_db\',','        \'NAME\': \''+db_name+'\',')
            newfile.write(newline)
   with open("settings.py", "a") as newfile:
      with open("seads/settings.py", "r") as oldfile:
         for line in oldfile:
            newline = line.replace('        \'USER\': \'django\'','        \'USER\': \''+db_user+'\'')
            newfile.write(newline)
   with open("settings.py", "a") as newfile:
      with open("seads/settings.py", "r") as oldfile:
         for line in oldfile:
            newline = line.replace('        \'PASSWORD\': \'teammantey\'','        \'PASSWORD\': \''+db_pswd+'\'')
            newfile.write(newline)
   with open("settings.py", "a") as newfile:
      with open("seads/settings.py", "r") as oldfile:
         for line in oldfile:
            newline = line.replace('        \'HOST\': \'django.seads.io\'','        \'HOST\': \''+db_host+'\'')
            newfile.write(newline)
   with open("settings.py", "a") as newfile:
      with open("seads/settings.py", "r") as oldfile:
         for line in oldfile:
            newline = line.replace('        \'PORT\': \'3306\'','        \'PORT\': \''+str(db_port)+'\'')
            newfile.write(newline)
   shutil.move("settings.py","seads/settings.py")

if __name__ == "__main__":
   main()
