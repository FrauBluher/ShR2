from django.db import models
from django.contrib.auth.models import User
from microdata.models import Device

# Create your models here.

class Notification(models.Model):
   description = models.CharField(max_length=300)
   #TODO add model fields to describe actions

   def __unicode__(self):
      return self.description

class UserSettings(models.Model):
   user = models.OneToOneField(User)
   notifications = models.ManyToManyField(Notification)


class UtilityCompany(models.Model):
   description = models.CharField(max_length=300)
   #TODO add model fields to describe actions

   def __unicode__(self):
      return self.description

class RatePlan(models.Model):
   description = models.CharField(max_length=300)
   #TODO add model fields to describe actions

   def __unicode__(self):
      return self.description

class Territory(models.Model):
   description = models.CharField(max_length=300)
   #TODO add model fields to describe actions

   def __unicode__(self):
      return self.description


class DeviceSettings(models.Model):
   device = models.OneToOneField(Device)
   utility_companies = models.ManyToManyField(UtilityCompany)
   rate_plans = models.ManyToManyField(RatePlan)
   territories = models.ManyToManyField(Territory)
