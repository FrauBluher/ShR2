from django.db import models
from django.conf import settings

from django_pandas.managers import DataFrameManager
# Create your models here.

class Appliance(models.Model):
    serial = models.IntegerField(unique=True)
    name = models.CharField(max_length=50, unique=True)

    def __unicode__(self):
        return self.name

class Device(models.Model):
    owner = models.ForeignKey(settings.AUTH_USER_MODEL, blank=True, null=True)
    secret_key = models.CharField(max_length=7, blank=True, null=True)
    serial = models.IntegerField(unique=True)
    name = models.CharField(max_length=30)
    zipcode = models.CharField(max_length=5, blank=True, null=True)
    private = models.BooleanField(default=False)
    registered = models.BooleanField(default=False)
    
    def __unicode__(self):
        return self.name    

class Event(models.Model):
    device = models.ForeignKey(Device, related_name='events')
    event_code = models.IntegerField(blank=True, null=True)
    appliance = models.ForeignKey(Appliance, related_name='appliance', blank=True, null=True)
    timestamp = models.PositiveIntegerField(help_text='13 digits, millisecond resolution')
    wattage = models.FloatField()
    
    objects = DataFrameManager()

    def __unicode__(self):
        return str(self.timestamp)

