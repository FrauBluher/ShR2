from django.db import models
from django.contrib.auth.models import User
from microdata.models import Device, Appliance
from django.conf import settings

# Create your models here.

class EventNotification(models.Model):
   description = models.CharField(
      max_length=300,
      help_text="Label to notification as shown to a user"
   )
   keyword = models.CharField(
      max_length=300,
      help_text="Keyword used to launch manage.py email_event",
   )
   watts_above_average = models.FloatField()
   period_of_time = models.FloatField(
      help_text="Period of time to watch for irregularity"
   )
   appliances_to_watch = models.ManyToManyField(Appliance)
   email_subject = models.CharField(max_length=300)
   email_body = models.FileField()
   
   def __unicode__(self):
      return self.description

class IntervalNotification(models.Model):
   description = models.CharField(
      max_length=300,
      help_text="Label to notification as shown to a user",
   )
   keyword = models.CharField(
      max_length=300,
      help_text="Keyword used to launch manage.py email_interval",
   )
   interval = models.CharField(
      max_length=300,
      help_text="Word descriptor for type of interval (i.e. Day). First letter capitalized.",
   )
   interval_adverb = models.CharField(
      max_length=300,
      help_text="Adverb of interval (i.e. Daily). First letter capitalized.",
   )
   time_delta = models.FloatField(
      help_text="Time in seconds between intervals",
   )
   email_subject = models.CharField(max_length=300)
   email_body = models.FileField()

   def __unicode__(self):
      return self.description

class Notification(models.Model):
   """
   DEPRECATED
   """
   user = models.OneToOneField(User)
   interval_notification = models.ManyToManyField(IntervalNotification)

   def __unicode__(self):
      return 'Notification '+str(self.pk)

class UserSettings(models.Model):
   user = models.OneToOneField(User)
   interval_notification = models.ManyToManyField(IntervalNotification)
   event_notification = models.ManyToManyField(EventNotification)


class UtilityCompany(models.Model):
   description = models.CharField(max_length=300)
   #TODO add model fields to describe actions

   class Meta:
        verbose_name_plural = "Utility Companies"

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

   class Meta:
        verbose_name_plural = "Territories"

   def __unicode__(self):
      return self.description


class DeviceWebSettings(models.Model):
   device = models.OneToOneField(Device)
   utility_companies = models.ManyToManyField(UtilityCompany)
   rate_plans = models.ManyToManyField(RatePlan)
   territories = models.ManyToManyField(Territory)

class DashboardSettings(models.Model):
   user = models.OneToOneField(User)
   stack = models.BooleanField(
      default=True,
      help_text="Specifies the default behavior for a graph: stacked or unstacked line chart"
   )
