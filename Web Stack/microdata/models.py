from django.db import models
from django.conf import settings
import random
import string
from django.core.exceptions import SuspiciousOperation
from influxdb import client as influxdb
from geoposition.fields import GeopositionField

# Create your models here.

class Appliance(models.Model):
   serial = models.IntegerField(unique=True)
   name = models.CharField(max_length=50, unique=True)

   def __unicode__(self):
      return self.name

class Device(models.Model):
   owner = models.ForeignKey(settings.AUTH_USER_MODEL, blank=True, null=True)
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
   event_code = models.IntegerField(blank=True, null=True)
   appliance = models.ForeignKey(Appliance, related_name='appliance', blank=True, null=True)
   timestamp = models.PositiveIntegerField(help_text='13 digits, millisecond resolution')
   wattage = models.FloatField(blank=True, null=True)
   current = models.FloatField(blank=True, null=True)
   voltage = models.FloatField(blank=True, null=True)

   def save(self, **kwargs):
      appliance = self.appliance
      if self.appliance == None:
         # link to the 'Unknown' appliance
         appliance = Appliance.objects.filter(serial=0)[0]
      db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
      data = []
      query = {}
      query['points'] = [[self.timestamp, appliance.name, self.wattage]]
      query['name'] = 'device.'+str(self.device.serial)
      query['columns'] = ['time', 'appliance', 'wattage']
      data.append(query)
      db.write_points(data)
      #super(Event, self).save()

   def __unicode__(self):
      return str(self.timestamp)

