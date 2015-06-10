from urllib2 import urlopen
import requests
import sys
import random
import json
import time
green = '\033[01;32m'
red = '\033[01;31m'
native = '\033[m'

my_ip=0

def check_website_access():
    sys.stdout.write("Checking if website accessible to the Internet......")
    r = requests.get('http://'+my_ip)
    if r.status_code == 200:
        sys.stdout.write(green+"\t Success\n"+native)
    else:
        sys.stdout.write(red+"\t Fail\n"+native)

def check_rest_functionality():
    print "Checking REST Framework functionality......"
    sys.stdout.write("\t POST /api/device-api/......")
    url = 'http://'+my_ip+'/api/device-api/'
    serial = random.randrange(1,65535)
    payload = {'serial':serial}
    headers = {'content-type': 'application/json'}
    r = requests.post(url, data=json.dumps(payload), headers=headers)
    if r.status_code == 201:
        sys.stdout.write(green+"\t Success\n"+native)
    else:
        sys.stdout.write(red+"\t Fail\n"+native)

    sys.stdout.write("\t GET /api/device-api/......")
    url = 'http://'+my_ip+'/api/device-api/'
    r = requests.get(url)
    if r.status_code == 200:
        sys.stdout.write(green+"\t Success\n"+native)
    else:
        sys.stdout.write(red+"\t Fail\n"+native)

    sys.stdout.write("\t GET /api/settings-api/"+str(serial)+"/")
    url = 'http://'+my_ip+'/api/settings-api/'+str(serial)+'/'
    r = requests.get(url)
    if r.status_code == 200:
        sys.stdout.write(green+"\t Success\n"+native)
    else:
        sys.stdout.write(red+"\t Fail\n"+native)

    sys.stdout.write("\t POST /api/event-api/")
    url = 'http://'+my_ip+'/api/event-api/'
    payload = {
        "device": "/api/device-api/"+str(serial)+"/",
        "time": [str(hex(int(time.time()*1000))), "1"],
        "dataPoints": [{"wattage": random.random()}],
    }
    r = requests.post(url, data=json.dumps(payload), headers=headers)
    if r.status_code == 201:
        sys.stdout.write(green+"\t\t Success\n"+native)
    else:
        sys.stdout.write(red+"\t\t Fail\n"+native)

    sys.stdout.write("\t DELETE /api/device-api/"+str(serial)+"/")
    url = 'http://'+my_ip+'/api/device-api/'+str(serial)+'/'
    r = requests.delete(url)
    if r.status_code == 204:
        sys.stdout.write(green+"\t Success\n"+native)
    else:
        sys.stdout.write(red+"\t Fail\n"+native)

def main():
    check_website_access()
    check_rest_functionality()



if __name__ == "__main__":
    my_ip = urlopen('http://ip.42.pl/raw').read()
    main()