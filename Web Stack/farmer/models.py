from django.db import models
from microdata.models import Device

# Create your models here.

class DeviceSettings(models.Model):
   """
   Describes the settings associated with a device.
   
   This is what is returned by a REST call to /api/settings-api/{serial}/.
   """
   device = models.OneToOneField(Device)
   """ Relational field to connect the settings to a device."""
   
   device_serial = models.IntegerField(unique=True, primary_key=True, editable=False)
   """ The serial of the device this model is linked to."""
   
   CHANNEL_CHOICES = (
      (1, 'Channel 1'),
      (2, 'Channel 2'),
      (3, 'Channel 3'),
      (4, 'Channel 4'),
   )
   main_channel = models.IntegerField(choices=CHANNEL_CHOICES,
                                      default=1)
   """
   Not currently in use. The idea here was to have the device multiplex
   its channels if it were only able to transmit one at a time. The SEAD
   Light is capable of transmitting on all channels.
   """
   
   SAMPLE_RATE_CHOICES = (
      (0, 125000  ),
      (1, 62500   ),
      (2, 31250   ),
      (3, 15625   ),
      (4, 7812.5  ),
      (5, 3906.25 ),
      (6, 1953.125),
      (7, 976.5625)
   )
   adc_sample_rate = models.IntegerField(choices=SAMPLE_RATE_CHOICES,
                                         default=5)
   """ The sample rates that can be chosen from the admin interface"""
   
   transmission_rate_milliseconds = models.IntegerField(default=1000)
   """ The period of time in milliseconds before packet transmission"""
   
   date_now = models.IntegerField(null=True,blank=True)
   """ Used by the SEAD Light to synchronize its RTC."""
   
   def save(self, **kwargs):
      self.device_serial = self.device.serial
      super(DeviceSettings, self).save()
      
   def __unicode__(self):
      return (self.device.name or 'NONAME') + ' settings'