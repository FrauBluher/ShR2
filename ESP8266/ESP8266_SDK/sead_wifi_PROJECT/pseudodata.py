import sys
import os
import time
import random
import serial

def main(args):
	#duration = int(args[0])\
	ser = serial.Serial('/dev/ttyUSB0', 115200, timeout = 1)
	print ser
	#ser.open()
	starttime = int(time.time() * 1000)
	while 1:
		ts = int(time.time() * 1000) - starttime
		watt = random.randint(20,100)
		writestring = '$SEDAT,' +str(watt)+','+str(ts)+'\r\n'
		ser.write(writestring)
		sys.stdout.write(ser.read(ser.inWaiting()))
		time.sleep(0.01666666666)
	ser.close()

if __name__ == "__main__":
   main(sys.argv[1:])

