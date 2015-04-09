from django.db import models
from django.conf import settings
import random
import string
from django.core.exceptions import SuspiciousOperation
from influxdb import client as influxdb
from geoposition.fields import GeopositionField
from paintstore.fields import ColorPickerField
import json

# Create your models here.

class Appliance(models.Model):
   serial = models.IntegerField(unique=True)
   name = models.CharField(max_length=50, unique=True)
   chart_color = ColorPickerField()

   def __unicode__(self):
      return self.name

class Device(models.Model):
   owner = models.ForeignKey(settings.AUTH_USER_MODEL, blank=True, null=True)
   ip_address = models.GenericIPAddressField()
   secret_key = models.CharField(max_length=7, blank=True, null=True, editable=False)
   serial = models.IntegerField(unique=True, primary_key=True)
   name = models.CharField(max_length=30, blank=True, null=True)
   position = GeopositionField(blank=True, null=True)
   #private = models.BooleanField(default=False)
   registered = models.BooleanField(default=False, editable=False)
   fanout_query_registered = models.BooleanField(default=False, editable=False)
    
   def save(self, **kwargs):
      if self.secret_key == None:
         secret_key =  ''.join(random.choice(string.digits) for i in range(3))
         secret_key += ''.join(random.choice(string.ascii_uppercase) for i in range(4))
         self.secret_key = secret_key
      if self.fanout_query_registered == False:
         db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
         serial = str(self.serial)
         db.query('select * from device.'+serial+' into device.'+str(self.serial)+'.[appliance]')
         db.query('select mean(wattage) from /^device.'+serial+'.*/ group by time(1y) into 1y.:series_name')
         db.query('select mean(wattage) from /^device.'+serial+'.*/ group by time(1M) into 1M.:series_name')
         db.query('select mean(wattage) from /^device.'+serial+'.*/ group by time(1w) into 1w.:series_name')
         db.query('select mean(wattage) from /^device.'+serial+'.*/ group by time(1d) into 1d.:series_name')
         db.query('select mean(wattage) from /^device.'+serial+'.*/ group by time(1h) into 1h.:series_name')
         db.query('select mean(wattage) from /^device.'+serial+'.*/ group by time(1m) into 1m.:series_name')
         db.query('select mean(wattage) from /^device.'+serial+'.*/ group by time(1s) into 1s.:series_name')
         self.fanout_query_registered = True
      super(Device, self).save()

   def delete(self, *args, **kwargs):
      db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
      serial = str(self.serial)
      series = db.query('list series')[0]['points']
      # delete series
      for s in series:
         if 'device.'+serial in s[1]:
            db.query('drop series '+s[1])
      # delete continuous queries
      queries = db.query('list continuous queries')[0]['points']
      for q in queries:
         if 'device.'+serial in q[2]:
            db.query('drop continuous query '+str(q[1]))
      super(Device, self).delete()
   
   def __unicode__(self):
      return self.name    

class Event(models.Model):
   device = models.ForeignKey(Device)
   dataPoints = models.CharField(max_length=1000,
                                 help_text='Expects a JSON encoded string of values:'+\
                                           '['+\
                                           '   {timestamp(int),\n'+\
                                           '   wattage(float, optional),\n'+\
                                           '   current(float, optional),\n'+\
                                           '   voltage(float, optional),\n'+\
                                           '   appliance_pk(int, optional),\n'+\
                                           '   event_code(int, optional)}\n'+\
                                           ',...]')
                                 
   def save(self, **kwargs):
      dataPoints = json.loads(self.dataPoints)
      for point in dataPoints:
         timestamp   = point.get('timestamp')
         wattage     = point.get('wattage')
         current     = point.get('current')
         voltage     = point.get('voltage')
         appliance_pk= point.get('appliance_pk')
         event_code  = point.get('event_code')
         if (timestamp and (wattage or current or voltage)):
            if appliance_pk == None:
               appliance = Appliance.objects.get(serial=0) # Unknown appliance
            else:
               appliance = Appliance.objects.get(pk=appliance_pk)
            db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
            data = []
            query = {}
            query['points'] = [[timestamp, appliance.name, wattage, current, voltage]]
            query['name'] = 'device.'+str(self.device.serial)
            query['columns'] = ['time', 'appliance', 'wattage', 'current', 'voltage']
            data.append(query)
            db.write_points(data)
      super(Event, self).save()
      
   def __unicode__(self):
      return str(self.device.name+':'+self.dataPoints)

      
