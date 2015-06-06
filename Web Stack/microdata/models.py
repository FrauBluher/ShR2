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
   """
   Describes a single Appliance. In the future, this model will have describing attributes that give the user helpful information when visualizing.
   """
   serial = models.IntegerField(unique=True)
   name = models.CharField(max_length=50, unique=True)
   chart_color = ColorPickerField()
   """ This field defines what color the chart will assign the appliance when it is being displayed. Modifiable via the admin interface. """

   def __unicode__(self):
      return self.name
      
# This model should not be registered to REST (admin only)
class CircuitType(models.Model):
   """
   Describes a Circuit Type. A Circuit Type is related to a list of :class:`microdata.models.Appliance` and acts as a set of objects to discover within a circuit.
   """
   name = models.CharField(max_length=50, unique=True)
   appliances = models.ManyToManyField(Appliance, blank=True)
   chart_color = ColorPickerField()
   """ This field defines what color the chart will assign the circuit when it is being displayed. Modifiable via the admin interface. """

   def __unicode__(self):
      return self.name

# This model should not be registered to REST (admin and webapp UI only)
class Circuit(models.Model):
   """
   Most likely deprecated.
   """
   circuittype = models.ForeignKey(CircuitType, null=True)
   name = models.CharField(max_length=50, null=True)

   def __unicode__(self):
      return self.name

class Device(models.Model):
   """
   Describes a single Device owned by :class:`settings.AUTH_USER_MODEL`.
   """

   owner = models.ForeignKey(settings.AUTH_USER_MODEL, blank=True, null=True)
   """A Foreign key relation. We use this relation to pair a device to a user on the web application."""

   share_with = models.ManyToManyField(settings.AUTH_USER_MODEL, blank=True, related_name='share_with')
   """ This field acts much like an owner, however share_with users cannot alter device settings."""

   ip_address = models.GenericIPAddressField(blank=True, null=True)
   """ This field is no longer actively used. This was a proof of concept for early device communication. Deprecated."""

   secret_key = models.CharField(max_length=7, blank=True, null=True, editable=False)
   """ This field is not currently in use, but the functionality exists. This field was intended to be used to pair a device to a user."""

   serial = models.IntegerField(unique=True, primary_key=True)
   """ The primary key of the model. This is what the device uses to interface to the API and how users currently pair a device."""

   name = models.CharField(max_length=30, blank=True, null=True)
   """ A non-unique name field for a device. This field is solely for user experience, it has no functional purpose."""

   position = GeopositionField(blank=True, null=True)
   """ A position field that essentially translates to coordinates on a map. Automatically generates a map on the admin interface."""

   registered = models.BooleanField(default=False, editable=False)
   """ Synonym for paired. This protects againts users trying to pair an already paired device."""

   fanout_query_registered = models.BooleanField(default=False, editable=False)
   """ A true/false that is set when a device is created indicating the continuous queries in the database have been registered."""

   channel_1 = models.ForeignKey(CircuitType, related_name='Channel 1+', blank=True, null=True)
   """ The first of three channels of the SEAD Light. This is a design flaw and should be instead a ManyToManyField."""

   channel_2 = models.ForeignKey(CircuitType, related_name='Channel 2+', blank=True, null=True)
   """ The second of three channels of the SEAD Light. This is a design flaw and should be instead a ManyToManyField."""

   channel_3 = models.ForeignKey(CircuitType, related_name='Channel 3+', blank=True, null=True)
   """ The third of three channels of the SEAD Light. This is a design flaw and should be instead a ManyToManyField."""

   data_retention_policy = models.IntegerField(help_text='Number of months of data to keep in database', default=12)
   """ The amount of time the data from this device can live in the database. Anything older will be archived to Amazon Glacier."""

   kilowatt_hours_monthly = models.FloatField(default=0, editable=False, help_text='Monthly killowatt consumption')
   """ A counter that is reset by a cron job once a month that keeps an accumulation of the total kwh this device has measured over the course of a month."""

   kilowatt_hours_daily = models.FloatField(default=0, editable=False, help_text='Daily killowatt consumption')
   """ A counter that is reset by a cron job once a day that keeps an accumulation of the total kwh this device has measured over the course of a day."""

   cost_daily = models.FloatField(default=0, editable=False, help_text='Daily cost')
   """ The total cost calculated by the server today. Much cheaper to keep this in the django database than the InfluxDB."""
   
   def save(self, **kwargs):
      """
      Custom save method.

      This function will do several things if it has not done so already:

      * Create a secret key. 3 digits followed by 4 letters. Not currently in use (could be used for pairing devices)
      * Register fanout queries in database.
      * Device name cannot be None, default is "Device <serial>".
      * Create a :class:`farmer.models.DeviceSettings` object.
      * Create a :class:`webapp.models.DeviceWebSettings` object.
      * Give default values to the channels.
      """
      if self.secret_key == None:
         secret_key =  ''.join(random.choice(string.digits) for i in range(3))
         secret_key += ''.join(random.choice(string.ascii_uppercase) for i in range(4))
         self.secret_key = secret_key
      if self.fanout_query_registered == False:
         db = influxdb.InfluxDBClient(settings.INFLUXDB_URI,8086,'root','root','seads')
         serial = str(self.serial)
         db.query('select * from device.'+serial+' into device.'+serial+'.[circuit_pk]')
         db.query('select sum(cost) from device.'+serial+' into cost.device.'+serial+'.[circuit_pk]')
         db.query('select mean(wattage) from /^device.'+serial+'.*/ group by time(1y) into 1y.:series_name')
         db.query('select mean(wattage) from /^device.'+serial+'.*/ group by time(1M) into 1M.:series_name')
         db.query('select mean(wattage) from /^device.'+serial+'.*/ group by time(1w) into 1w.:series_name')
         db.query('select mean(wattage) from /^device.'+serial+'.*/ group by time(1d) into 1d.:series_name')
         db.query('select mean(wattage) from /^device.'+serial+'.*/ group by time(1h) into 1h.:series_name')
         db.query('select mean(wattage) from /^device.'+serial+'.*/ group by time(1m) into 1m.:series_name')
         db.query('select mean(wattage) from /^device.'+serial+'.*/ group by time(1s) into 1s.:series_name')
         db.query('select sum(cost) from "device.'+serial+'" into cost.device.'+serial)
         self.fanout_query_registered = True
      if self.name == '':
         self.name = "Device "+str(self.serial)
      from farmer.models import DeviceSettings
      device_settings = DeviceSettings.objects.filter(device=self)
      if not device_settings:
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
      """
      Custom delete method.

      Drop the series from the influxdb database.
      """
      db = influxdb.InfluxDBClient(settings.INFLUXDB_URI,8086,'root','root','seads')
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
   """
   Generic class to catch the Event REST Packets from devices. Relates to a :class:`microdata.models.Device`.

   These models are not stored on the Django database since they are converted to InfluxDB.
   """
   device = models.ForeignKey(Device)
   """ This is the relation between the device and its data. This is established when the device specifies its hyperlinked model via the REST call."""

   dataPoints = models.CharField(max_length=1000,
                                 help_text='Expects a JSON encoded string of values:'+\
                                           '{time(int, milliseconds),frequency(int, Hz),\n'+\
                                           ' ['+\
                                           '    {wattage(float, optional),\n'+\
                                           '    current(float, optional),\n'+\
                                           '    voltage(float, optional),\n'+\
                                           '    event_code(int, optional),\n'+\
                                           '    channel(int, optional)}\n'+\
                                           ' ,...]')
   """ An array of undefined size containing measured values for the server. The data points are numbered `0 ... n ... j` where nth data point has a timestamp of `start_time + n * period` and `j` is undefined."""

   start = models.IntegerField()
   """ The start time, in milliseconds, of the first data point in the packet. Used to calculate offset of all proceeding points."""

   frequency = models.IntegerField()
   """ The frequency, in Hertz, of the packet's data points. Used to calculate the offset of all points."""

   query = models.CharField(max_length=1000)
   """ Deprecated. This field is useful for debugging the REST requests, but since the model is not saved, this is a volatile field."""

   def save(self, **kwargs):
      """
      Custom save method.

      This method is the powerhouse of the API. It can take an array of data points from a device and convert them into database entries in InfluxDB.

      The method will also keep a running count of how many kwh have been consumed this day and this month. If it exceeds the allotted kwh for
      the device's tier, advance the tier a level.

      If the data coming in is sufficiently in the past such that the database will not calculate its mean value, refresh the query
      to trigger a backfill of the data.

      When a model is being saved, it has already been created by :class:`microdata.views.EventViewSet`.

      The Event is parsed as follows::

         start = self.start
         frequency = self.frequency
         count = 0

         for point in dataPoints:
            time = start + count * (1/frequency)
            db.write_points(time, wattage)

      """
      dataPoints = json.loads(self.dataPoints)
      db = influxdb.InfluxDBClient(settings.INFLUXDB_URI,8086,'root','root','seads')
      self.dataPoints = dataPoints
      count = 0
      now = time.time()
      timestamp = now*1000
      query  = {}
      query['points'] = []
      for point in dataPoints:
         wattage      = point.get('wattage')
         if wattage == 0xFFFF: continue # Issue where a device sends overflow. Ignore for now.
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
         self.device.cost_daily += cost
         
         if (timestamp and (wattage or current or voltage)):
            query['points'].append([timestamp, wattage, current, voltage, circuit_pk, cost])
            
      data = []
      query['name'] = 'device.'+str(self.device.serial)
      query['columns'] = ['time', 'wattage', 'current', 'voltage', 'circuit_pk', 'cost']
      data.append(query)
      self.query += str(data)
      db.write_points(data, time_precision="ms")
            
      # If data is older than the present, must backfill fanout queries by reloading the continuous query.
      # Could be fixed with an influxdb update
      # https://github.com/influxdb/influxdb/issues/510
      now = time.time()
      last_timestamp = (self.start + ((1.0/self.frequency)*count*1000))/1000
      existing_queries = db.query('list continuous queries')[0]['points']
      new_queries = []
      #if last_timestamp < now - 1:
      #   new_queries.append('select mean(wattage) from /^device.'+str(self.device.serial)+'.*/ group by time(1s) into 1s.:series_name')
      if last_timestamp < now - 60:
         new_queries.append('select mean(wattage) from /^device.'+str(self.device.serial)+'.*/ group by time(1m) into 1m.:series_name')
      if last_timestamp < now - 3600:
         new_queries.append('select mean(wattage) from /^device.'+str(self.device.serial)+'.*/ group by time(1h) into 1h.:series_name')
      if last_timestamp < now - 86400:
         new_queries.append('select mean(wattage) from /^device.'+str(self.device.serial)+'.*/ group by time(1d) into 1d.:series_name')
      if last_timestamp < now - 86400*7:
         new_queries.append('select mean(wattage) from /^device.'+str(self.device.serial)+'.*/ group by time(1w) into 1w.:series_name')
      if last_timestamp < now - 86400*days_this_month:
         new_queries.append('select mean(wattage) from /^device.'+str(self.device.serial)+'.*/ group by time(1M) into 1M.:series_name')
      if last_timestamp < now - 86400*days_this_month*12:
         new_queries.append('select mean(wattage) from /^device.'+str(self.device.serial)+'.*/ group by time(1y) into 1y.:series_name')
      # drop old continuous query, add new one. Essentially a refresh.
      for new_query in new_queries:
         for existing_query in existing_queries:
            if new_query == existing_query[2]:
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
