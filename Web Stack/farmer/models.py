from django.db import models
from microdata.models import Device

# Create your models here.

class DeviceSettings(models.Model):
   device = models.OneToOneField(Device)
   device_serial = models.IntegerField(unique=True, primary_key=True, editable=False)
   CHANNEL_CHOICES = (
      (1, 'Channel 1'),
      (2, 'Channel 2'),
      (3, 'Channel 3'),
      (4, 'Channel 4'),
   )
   main_channel = models.IntegerField(max_length=1,
                                   choices=CHANNEL_CHOICES,
                                   default=1)
   transmission_rate_milliseconds = models.IntegerField(default=1000)
   date_now = models.IntegerField(null=True,blank=True)
   def save(self, **kwargs):
      self.device_serial = self.device.serial
      super(DeviceSettings, self).save()
      
   def __unicode__(self):
      return (self.device.name or 'NONAME') + ' settings'