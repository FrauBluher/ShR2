from django.db import models
from django.conf import settings
import random
import string
from influxdb import client as influxdb

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
   zipcode = models.CharField(max_length=5, blank=True, null=True)
   private = models.BooleanField(default=False)
   registered = models.BooleanField(default=False)
   fanout_query_registered = models.BooleanField(default=False, editable=False)
    
   def save(self, **kwargs):
      if self.secret_key == None:
         secret_key =  ''.join(random.choice(string.digits) for i in range(3))
         secret_key += ''.join(random.choice(string.ascii_uppercase) for i in range(4))
         self.secret_key = secret_key
      if self.fanout_query_registered == False:
         db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
         db.query('select * from device.'+str(self.serial)+' into device.'+str(self.serial)+'.[appliance]')
         self.fanout_query_registered = True
      super(Device, self).save()
   
   def __unicode__(self):
      return self.name    

class Event(models.Model):
   device = models.ForeignKey(Device)
   event_code = models.IntegerField(blank=True, null=True)
   appliance = models.ForeignKey(Appliance, related_name='appliance', blank=True, null=True)
   timestamp = models.PositiveIntegerField(help_text='13 digits, millisecond resolution')
   wattage = models.FloatField()

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

