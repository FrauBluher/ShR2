import shutil

def main():
	port = raw_input('Port for MySQL Server (3306): ')
	if port == '': port = 3306
	with open("my.cnf", "w") as newfile:
		with open("/etc/mysql/my.cnf", "r") as oldfile:
			for line in oldfile:
				newline = line.replace('port		= 3306','port		= '+str(port))
				newline = line.replace('bind-address		= 127.0.0.1','#bind-address		= 127.0.0.1')
				newfile.write(newline)
	shutil.move("my.cnf","/etc/mysql/my.cnf")

if __name__ == "__main__":
	main()
