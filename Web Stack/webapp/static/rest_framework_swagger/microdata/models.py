from django.db import models
from django.conf import settings

# Create your models here.

class Device(models.Model):
    owner = models.ForeignKey(settings.AUTH_USER_MODEL)
    serial = models.IntegerField()
    name = models.CharField(max_length=30)
    zipcode = models.CharField(max_length=5)
    private = models.BooleanField(default=False)
    
    def __unicode__(self):
        return self.name    

class Event(models.Model):
    device = models.ForeignKey(Device, related_name='events')
    event_code = models.IntegerField(blank=True, null=True)
    wattage = models.FloatField()
    timestamp = models.IntegerField()

    def __unicode__(self):
        return "Event: " + str(self.timestamp)

