from django.db import models
from django.conf import settings
import random
import string
from django.core.exceptions import SuspiciousOperation
from influxdb.influxdb08 import client as influxdb
from geoposition.fields import GeopositionField
from paintstore.fields import ColorPickerField
import json
import time
from datetime import datetime
from calendar import monthrange


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
   chart_color = ColorPickerField()

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
   share_with = models.ManyToManyField(settings.AUTH_USER_MODEL, blank=True, related_name='share_with')
   ip_address = models.GenericIPAddressField(blank=True, null=True)
   secret_key = models.CharField(max_length=7, blank=True, null=True, editable=False)
   serial = models.IntegerField(unique=True, primary_key=True)
   name = models.CharField(max_length=30, blank=True, null=True)
   position = GeopositionField(blank=True, null=True)
   registered = models.BooleanField(default=False, editable=False)
   fanout_query_registered = models.BooleanField(default=False, editable=False)
   channel_1 = models.ForeignKey(CircuitType, related_name='Channel 1+', blank=True, null=True)
   channel_2 = models.ForeignKey(CircuitType, related_name='Channel 2+', blank=True, null=True)
   channel_3 = models.ForeignKey(CircuitType, related_name='Channel 3+', blank=True, null=True)
   data_retention_policy = models.IntegerField(help_text='Number of months of data to keep in database', default=12)
   kilowatt_hours_monthly = models.FloatField(default=0, editable=False, help_text='Monthly killowatt consumption')
   kilowatt_hours_daily = models.FloatField(default=0, editable=False, help_text='Daily killowatt consumption')
   
   def save(self, **kwargs):
      if self.secret_key == None:
         secret_key =  ''.join(random.choice(string.digits) for i in range(3))
         secret_key += ''.join(random.choice(string.ascii_uppercase) for i in range(4))
         self.secret_key = secret_key
      if self.fanout_query_registered == False:
         db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
         serial = str(self.serial)
         db.query('select * from device.'+serial+' into device.'+serial+'.[channel_pk]')
         db.query('select sum(cost) from device.'+serial+' into cost.device.'+serial+'.[channel_pk]')
         db.query('select mean(wattage) from /^1M.device.'+serial+'.*/ group by time(1y) into 1y.:series_name')
         db.query('select mean(wattage) from /^1w.device.'+serial+'.*/ group by time(1M) into 1M.:series_name')
         db.query('select mean(wattage) from /^1d.device.'+serial+'.*/ group by time(1w) into 1w.:series_name')
         db.query('select mean(wattage) from /^1h.device.'+serial+'.*/ group by time(1d) into 1d.:series_name')
         db.query('select mean(wattage) from /^1m.device.'+serial+'.*/ group by time(1h) into 1h.:series_name')
         db.query('select mean(wattage) from /^1s.device.'+serial+'.*/ group by time(1m) into 1m.:series_name')
         db.query('select mean(wattage) from /^device.'+serial+'.*/ group by time(1s) into 1s.:series_name')
         self.fanout_query_registered = True
      if self.name == '':
         self.name = "Device "+str(self.serial)
      from farmer.models import DeviceSettings
      settings = DeviceSettings.objects.filter(device=self)
      if not settings:
         DeviceSettings.objects.create(device=self)
      from webapp.models import DeviceWebSettings, Tier, UtilityCompany, RatePlan, Territory
      websettings = DeviceWebSettings.objects.filter(device=self)
      if len(websettings) == 0:
         DeviceWebSettings.objects.create(device=self)
         self.devicewebsettings.utility_companies.add(UtilityCompany.objects.get(pk=1))
         self.devicewebsettings.rate_plans.add(RatePlan.objects.get(pk=4))
         self.devicewebsettings.territories.add(Territory.objects.get(pk=1))
         self.devicewebsettings.current_tier = Tier.objects.get(pk=1)
         self.devicewebsettings.save()
      if self.channel_1 == None:
         self.channel_1 = CircuitType.objects.get(pk=1)
      if self.channel_2 == None:
         self.channel_2 = CircuitType.objects.get(pk=3)
      if self.channel_3 == None:
         self.channel_3 = CircuitType.objects.get(pk=4)
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
                                           '{time(int, milliseconds),frequency(int, Hz),\n'+\
                                           ' ['+\
                                           '    {wattage(float, optional),\n'+\
                                           '    current(float, optional),\n'+\
                                           '    voltage(float, optional),\n'+\
                                           '    appliance_pk(int, optional),\n'+\
                                           '    event_code(int, optional),\n'+\
                                           '    channel(int, optional)}\n'+\
                                           ' ,...]')
   start = models.IntegerField()
   frequency = models.IntegerField()
   query = models.CharField(max_length=1000)

   def save(self, **kwargs):
      dataPoints = json.loads(self.dataPoints)
      self.dataPoints = dataPoints
      count = 0
      now = time.time()
      timestamp = now*1000
      for point in dataPoints:
         wattage      = point.get('wattage')
         current      = point.get('current')
         voltage      = point.get('voltage')
         appliance_pk = point.get('appliance_pk')
         event_code   = point.get('event_code')
         channel      = point.get('channel', 7)
         circuit_pk = self.device.channel_1.pk
         if channel == 2: circuit_pk = self.device.channel_2.pk
         elif channel == 3: circuit_pk = self.device.channel_3.pk
         # timestamp is millisecond resolution always
         timestamp = self.start + ((1.0/self.frequency)*count*1000)
         count += 1

         kwh = 0.0
         kwh = (wattage/1000.0)*(1.0/self.frequency)*(1/3600.0)
         self.device.kilowatt_hours_monthly += kwh
         self.device.kilowatt_hours_daily += kwh
         self.device.save()

         tier_dict = {}
         tier_dict['name'] = "tier.device."+str(self.device.serial)
         tier_dict['columns'] = ['tier_level']
         tier_dict['points'] = []

         # Calculate percent of baseline to get tier level
         # Start by determining current time of year
         this_year = datetime.now().year
         this_month = datetime.now().month
         days_this_month = monthrange(this_year,this_month)[1]
         summer_start = datetime(year=this_year,month=self.device.devicewebsettings.territories.all()[0].summer_start,day=1)
         winter_start = datetime(year=this_year,month=self.device.devicewebsettings.territories.all()[0].winter_start,day=1)
         current_season = 'summer'
         if (summer_start <= datetime.now() < winter_start) == False:
            current_season = 'winter'
         # check if we need to upgrade a tier. If at max tier, do nothing.
         if (self.device.devicewebsettings.current_tier.max_percentage_of_baseline != None):
            max_kwh_for_tier = (self.device.devicewebsettings.current_tier.max_percentage_of_baseline/100.0)*self.device.devicewebsettings.territories.all()[0].summer_rate*days_this_month
            if current_season == 'winter':
               max_kwh_for_tier = (self.device.devicewebsettings.current_tier.max_percentage_of_baseline/100.0)*self.device.devicewebsettings.territories.all()[0].winter_rate*days_this_month
            if (self.device.kilowatt_hours_monthly > max_kwh_for_tier):
               current_tier = self.device.devicewebsettings.current_tier
               self.device.devicewebsettings.current_tier = Tier.objects.get(tier_level=(current_tier.tier_level + 1))
               self.device.devicewebsettings.save()
               tier_dict['points'].append([current_tier.tier_level + 1])
               db.write_points([tier_dict])
         cost = self.device.devicewebsettings.current_tier.rate * kwh
         
         if (timestamp and (wattage or current or voltage)):
            if appliance_pk == None:
               appliance = Appliance.objects.get(serial=0) # Unknown appliance
            else:
               appliance = Appliance.objects.get(pk=appliance_pk)
            db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
            data = []
            query = {}
            query['points'] = [[timestamp, wattage, current, voltage, circuit_pk, cost]]
            query['name'] = 'device.'+str(self.device.serial)
            query['columns'] = ['time', 'wattage', 'current', 'voltage', 'circuit_pk', 'cost']
            data.append(query)
            self.query += str(data)
            db.write_points(data, time_precision="ms")
            
      # If data is older than the present, must backfill fanout queries by reloading the continuous query.
      # https://github.com/influxdb/influxdb/issues/510
      now = time.time()
      timestamp /= 1000 # convert to seconds. We don't care that much about accuracy at this point.
      existing_queries = db.query('list continuous queries')[0]['points']
      new_queries = []
      if timestamp < now:
         new_queries.append('select mean(wattage) from /^device.'+str(self.device.serial)+'.*/ group by time(1s) into 1s.:series_name')
      if timestamp < now - 60:
         new_queries.append('select mean(wattage) from /^1s.device.'+str(self.device.serial)+'.*/ group by time(1m) into 1m.:series_name')
      if timestamp < now - 3600:
         new_queries.append('select mean(wattage) from /^1m.device.'+str(self.device.serial)+'.*/ group by time(1h) into 1h.:series_name')
      if timestamp < now - 86400:
         new_queries.append('select mean(wattage) from /^1h.device.'+str(self.device.serial)+'.*/ group by time(1d) into 1d.:series_name')
      if timestamp < now - 604800:
         new_queries.append('select mean(wattage) from /^1d.device.'+str(self.device.serial)+'.*/ group by time(1w) into 1w.:series_name')
      if timestamp < now - 86400*days_this_month:
         new_queries.append('select mean(wattage) from /^1w.device.'+str(self.device.serial)+'.*/ group by time(1M) into 1M.:series_name')
      if timestamp < now - 86400*days_this_month*12:
         new_queries.append('select mean(wattage) from /^1M.device.'+str(self.device.serial)+'.*/ group by time(1y) into 1y.:series_name')
      # drop old continuous query, add new one. Essentially a refresh.
      for new_query in new_queries:
         for existing_query in existing_queries:
            if new_query in existing_query[2]:
               db.query('drop continuous query '+str(existing_query[1]))
               db.query(new_query)
      
      # Check to see if the tier series has been initialized. This should only need to happen once ever.
      try:
         db.query('select * from tier.device.'+str(self.device.serial))
      except:
         tier_dict = {}
         tier_dict['name'] = "tier.device."+str(self.device.serial)
         tier_dict['columns'] = ["tier_level"]
         tier_dict['points'] = [[str(self.device.devicewebsettings.current_tier.tier_level)]]
         db.write_points([tier_dict])

            

      # No need to save the event in the Django database.
      #super(Event, self).save()
      
   def __unicode__(self):
      return str(self.device.__unicode__()+':'+self.dataPoints)
