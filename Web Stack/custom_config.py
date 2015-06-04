import os
from os.path import expanduser

def main():
	cwd = os.getcwd()
	with open("/srv/nginx.conf", "w") as newfile:
		with open("deploy/nginx.conf", "r") as oldfile:
			for line in oldfile:
				newline = line.replace('           alias "/home/ubuntu/seads/Web Stack/webapp/static/";','           alias "'+cwd+'/webapp/static/";')
				newfile.write(newline)
	with open("/srv/nginx.conf", "w") as newfile:
		with open("deploy/nginx.conf", "r") as oldfile:
			for line in oldfile:
				newline = line.replace('           alias "/home/ubuntu/seads/Web Stack/documentation/_build/html";','           alias "'+cwd+'documentation/_build/html";')
				newfile.write(newline)

	with open("/srv/uwsgi.ini", "w") as newfile:
		with open("deploy/uwsgi.ini", "r") as oldfile:
			for line in oldfile:
				newline = line.replace('chdir=/home/ubuntu/seads/Web Stack/','chdir='+cwd)
				newfile.write(newline)
				

if __name__ == "__main__":
	main()