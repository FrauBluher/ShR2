import sys
from optparse import OptionParser
import requests
import json
import time
import datetime
import random
from progressbar import ProgressBar, Bar, Percentage, ETA, RotatingMarker

class Appliance():
   def __init__(self, pk, name, avg):
      self.pk = pk
      self.name = name
      self.avg = avg
      self.hyperlink = '/api/appliance-api/'+str(self.pk)+'/'

class Device():
   pass

def appliance_gen():
   appliances = []
   names = [
      {'name':'Unknown','avg':700},
   ]
   #for i in range(1, 6):
   #   appliances.append(Appliance(i, names[i-1]['name'], names[i-1]['avg']))
   appliances.append(Appliance(5, names[0]['name'], names[0]['avg']))
   return appliances

def main():
   parser = OptionParser()
   parser.add_option("-e", "--end", dest="end", type="int",
                     help="define end time (in UTC seconds) of script", metavar="timestamp")
   parser.add_option("-s", "--size", dest="size", type="int",
                     help="define the number of packets to send")
   parser.add_option("-d", "--device", dest="serial", type="int",
                     help="serial of device to send from")
   parser.add_option("-f", "--frequency", dest="frequency", type="int",
                     help="packets per second", metavar="frequency")
   parser.add_option("-v", "--verbose", action="store_true", dest="verbose")
   (options, args) = parser.parse_args()
   verbose = options.verbose
   
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
   url = 'http://seads.brabsmit.com/api/device-api/'+str(serial)+'/'
   r = requests.get(url)
   device = Device()
   if r.status_code == 200:
      data = json.loads(r.text)
      name = data['name']
      owner = data['owner']
      data_serial = data['serial']
      device.name = name
      device.owner = owner
      device.serial = data_serial
      print "Device found!"
      print "name: "+name
      print "owner: "+owner
      print "serial: "+str(data_serial)
   else:
      print "Device not found. Exiting."
      sys.exit(1)
   appliances = appliance_gen()
   
   if end:
      size = int(end-time.time())
   
   if size is not None:
      duration = size/options.frequency
      print "Starting data generation."
      print "Total number of packets: "+str(size)
      print "Time until done: "+str(duration)+" seconds"
      print "ETA: "+datetime.datetime.fromtimestamp(
            int(time.time()+duration-28800)
         ).strftime('%Y-%m-%d %H:%M:%S')
      url = 'http://seads.brabsmit.com/api/event-api/'
      headers = {'content-type': 'application/json'}
      widgets = [device.name+': ', Percentage(), ' ', Bar(), ' ', ETA()]
      pbar = ProgressBar(widgets=widgets, maxval=duration).start()
      ts = time.time()
      dataPoints = []
      points_per_packet = options.frequency
      point_count = 0
      for i in range(size):
         for appliance in appliances:
            dataPoints.append({
                                "wattage": random.randint(int(appliance.avg/1.3), int(appliance.avg*1.3)),
                              })
         point_count += 1
         if point_count == points_per_packet:
            payload = {
               'device': '/api/device-api/'+str(data_serial)+'/',
               'time': [hex(int(ts)), str(options.frequency)],
               'dataPoints': dataPoints
            }
            r = requests.post(url, data=json.dumps(payload), headers=headers)
            point_count = 0
            ts = time.time()
            if r.status_code != 201:
               print 'Error: event not created'
               print json.dumps(payload)
               print r.text
               sys.exit(1)
            if verbose: pbar.update(i)
         if options.frequency:
            time.sleep(1/options.frequency)
         else:
            time.sleep(1)
      pbar.finish()


if __name__ == "__main__":
   main()