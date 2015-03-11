import sys
import os
import time
import random

def main(args):
   duration = int(args[0])
   for i in range(duration):
      ts = int(time.time())
      watt = random.randint(20,100)
      os.system('echo "\$SEDAT,'+str(watt)+','+str(ts)+'\r\n" | sudo socat - /dev/ttyUSB0,raw,echo=0,crnl')
      time.sleep(1)
      os.system('echo "\n" | sudo socat - /dev/ttyUSB0,raw,echo=0,crnl')
      time.sleep(1)

if __name__ == "__main__":
   main(sys.argv[1:])

