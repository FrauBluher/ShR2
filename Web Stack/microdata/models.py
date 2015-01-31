from django.db import models
from django.conf import settings
import random
import string

from django_pandas.managers import DataFrameManager
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
    
    def save(self, **kwargs):
      if self.secret_key == None:
         secret_key =  ''.join(random.choice(string.digits) for i in range(3))
         secret_key += ''.join(random.choice(string.ascii_uppercase) for i in range(4))
         self.secret_key = secret_key
      super(Device, self).save()
    
    def __unicode__(self):
        return self.name    

class Event(models.Model):
    device = models.ForeignKey(Device)
    event_code = models.IntegerField(blank=True, null=True)
    appliance = models.ForeignKey(Appliance, related_name='appliance', blank=True, null=True)
    timestamp = models.PositiveIntegerField(help_text='13 digits, millisecond resolution')
    wattage = models.FloatField()
    
    objects = DataFrameManager()

    def save(self, **kwargs):
        if self.appliance == None:
            # link to the 'Unknown' appliance
            appliance = Appliance.objects.filter(serial=0)
        super(Event, self).save()

    def __unicode__(self):
        return str(self.timestamp)

