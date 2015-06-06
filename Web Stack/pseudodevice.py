import sys
from optparse import OptionParser
import requests
import json
import time
import datetime
import random

class Device():
   pass


def main():
   """
   The purpose of this function is to mimic the behavior of a device
   by interacting with the API. This is to test the functionality
   of the REST Framework.
   
   **Options**
   
       ``-e`` ``--end``
       
           int - define end time (in UTC seconds) of script
           
       ``-s`` ``--size``
       
           define the number of packets to send
           
       ``-d`` ``--device``
       
           serial of device to send from
   """
   
   channel_map = {
      1:400,
      #2:200,
      #3:1000,
   }
   parser = OptionParser()
   parser.add_option("-e", "--end", dest="end", type="int",
                     help="define end time (in UTC seconds) of script", metavar="timestamp")
   parser.add_option("-s", "--size", dest="size", type="int",
                     help="define the number of packets to send")
   parser.add_option("-d", "--device", dest="serial", type="int",
                     help="serial of device to send from")
   (options, args) = parser.parse_args()
   
   print "Starting pseudodevice..."
   end = options.end
   size = options.size
   serial = options.serial
   if serial is None or end is None and size is None:
      if serial is None:
         print "No device serial specified. Exiting."
      else:
         print "No end time or size specified. Exiting."
      sys.exit(1)
      
   print "Checking if device exists on server..."
   url = 'http://ec2-52-24-127-93.us-west-2.compute.amazonaws.com/api/device-api/'+str(serial)+'/'
   r = requests.get(url)
   device = Device()
   if r.status_code == 200:
      data = json.loads(r.text)
      device.name = data.get('name','')
      device.owner = data.get('owner','')
      device.serial = data.get('serial','')
      print "Device found!"
      print "name: "+device.name
      print "owner: "+device.owner
      print "serial: "+str(device.serial)
   else:
      print "Device not found. Exiting."
      print r.status_code
      print r.text
      sys.exit(1)
   
   if end:
      size = int(end-time.time())
   
   if size is not None:
      duration = size
      print "Starting data generation."
      print "Total number of packets: "+str(size)
      print "Time until done: "+str(duration)+" seconds"
      print "ETA: "+datetime.datetime.fromtimestamp(
            int(time.time()+duration-28800)
         ).strftime('%Y-%m-%d %H:%M:%S')
      url = 'http://ec2-52-24-127-93.us-west-2.compute.amazonaws.com/api/event-api/'
      headers = {'content-type': 'application/json'}
      dataPoints = []
      for i in range(size):
         for channel in {1, }:#3, 4}:
            channel_map[channel] += random.uniform(-20,20)
            if channel_map[channel] < 0: channel_map[channel] = 0
            if channel_map[channel] > 2000: channel_map[channel] = 2000
            dataPoints.append({
               "wattage": channel_map[channel],
               "channel": channel,
            })
         payload = {
            "device": "/api/device-api/"+str(device.serial)+"/",
            "time": [str(hex(int(time.time()*1000))), "1"],
            "dataPoints": dataPoints,
         }
         r = requests.post(url, data=json.dumps(payload), headers=headers)
         if r.status_code != 201:
            print 'Error: event not created'
            print r.text
            continue
         time.sleep(1)


if __name__ == "__main__":
   main()
