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
      
# This model should not be registered to REST (admin only)
class CircuitType(models.Model):
   name = models.CharField(max_length=50, unique=True)
   appliances = models.ManyToManyField(Appliance, blank=True)

   def __unicode__(self):
      return self.name

# This model should not be registered to REST (admin and webapp UI only)
class Circuit(models.Model):
   circuittype = models.ForeignKey(CircuitType, null=True)
   name = models.CharField(max_length=50, null=True)

   def __unicode__(self):
      return self.name


class Device(models.Model):
   owner = models.ForeignKey(settings.AUTH_USER_MODEL, blank=True, null=True)
   ip_address = models.GenericIPAddressField(blank=True, null=True)
   secret_key = models.CharField(max_length=7, blank=True, null=True, editable=False)
   serial = models.IntegerField(unique=True, primary_key=True)
   name = models.CharField(max_length=30, blank=True, null=True)
   position = GeopositionField(blank=True, null=True)
   registered = models.BooleanField(default=False, editable=False)
   fanout_query_registered = models.BooleanField(default=False, editable=False)
   channel_0 = models.ForeignKey(CircuitType, related_name='Channel 0')
   channel_1 = models.ForeignKey(CircuitType, related_name='Channel 1')
   channel_2 = models.ForeignKey(CircuitType, related_name='Channel 2')
    
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
      if self.name == '':
         self.name = "Device "+str(self.serial)
      from farmer.models import DeviceSettings
      settings = DeviceSettings.objects.filter(device=self)
      if not settings:
         DeviceSettings.objects.create(device=self)
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
      return self.name or 'NONAME_'+str(self.serial)


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
                                           '   event_code(int, optional),\n'+\
                                           '   channel(int, optional)}\n'+\
                                           ',...]')
                                 
   def save(self, **kwargs):
      dataPoints = json.loads(self.dataPoints)
      self.dataPoints = dataPoints
      for point in dataPoints:
         timestamp   = point.get('timestamp')
         wattage     = point.get('wattage')
         current     = point.get('current')
         voltage     = point.get('voltage')
         appliance_pk= point.get('appliance_pk')
         event_code  = point.get('event_code')
         channel     = point.get('channel', 1)
         if (timestamp and (wattage or current or voltage)):
            if appliance_pk == None:
               appliance = Appliance.objects.get(serial=0) # Unknown appliance
            else:
               appliance = Appliance.objects.get(pk=appliance_pk)
            db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
            data = []
            query = {}
            query['points'] = [[timestamp, appliance.name, wattage, current, voltage, channel]]
            query['name'] = 'device.'+str(self.device.serial)
            query['columns'] = ['time', 'appliance', 'wattage', 'current', 'voltage', 'channel']
            data.append(query)
            db.write_points(data)

      super(Event, self).save()
      
   def __unicode__(self):
      return str(self.device.__unicode__()+':'+self.dataPoints)
