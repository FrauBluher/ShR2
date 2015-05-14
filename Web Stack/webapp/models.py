from django.db import models
from django.contrib.auth.models import User
from microdata.models import Device, Appliance
from django.conf import settings
from recurrence.fields import RecurrenceField
from paintstore.fields import ColorPickerField

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
   email_body = models.FileField(
      help_text="Template file for email body. {{ x }} denotes template variable"
   )
   
   def __unicode__(self):
      return self.description

class IntervalNotification(models.Model):
   description = models.CharField(
      max_length=300,
      help_text="Label to notification as shown to a user",
   )
   recurrences = RecurrenceField(blank=True, null=True)
   email_subject = models.CharField(max_length=300)
   email_body = models.FileField(
      help_text="Template file for email body. {{ x }} denotes template variable"
   )

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
   interval_notification = models.ManyToManyField(IntervalNotification, blank=True)
   event_notification = models.ManyToManyField(EventNotification, blank=True)


class UtilityCompany(models.Model):
   description = models.CharField(max_length=300)

   class Meta:
        verbose_name_plural = "Utility Companies"

   def __unicode__(self):
      return self.description


class RatePlan(models.Model):
   utility_company = models.ForeignKey(UtilityCompany)
   description = models.CharField(max_length=300)
   data_source = models.URLField()
   min_charge_rate = models.FloatField(help_text="$ Per meter per day")
   california_climate_credit = models.FloatField(help_text="$ Per household, per semi-annual payment occurring in the April and October bill cycles")

   def __unicode__(self):
      return self.utility_company.__unicode__() + ": " + self.description

class Tier(models.Model):
   rate_plan = models.ForeignKey(RatePlan)
   tier_level = models.IntegerField(blank=True, null=True)
   max_percentage_of_baseline = models.FloatField(help_text="blank for no maximum",blank=True, null=True)
   rate = models.FloatField(help_text="$",blank=True, null=True)
   chart_color = ColorPickerField()

   def __unicode__(self):
      return 'Tier ' + str(self.tier_level)

class Territory(models.Model):
   rate_plan = models.ForeignKey(RatePlan)
   description = models.CharField(max_length=300)
   data_source = models.URLField()
   summer_start = models.IntegerField(blank=True,null=True,help_text="Specify Month of year")
   winter_start = models.IntegerField(blank=True,null=True,help_text="Specify Month of year")
   summer_rate = models.FloatField(help_text="Baseline quantity (kWh per day)")
   winter_rate = models.FloatField(help_text="Baseline quantity (kWh per day)")

   class Meta:
        verbose_name_plural = "Territories"

   def __unicode__(self):
      return self.description


class DeviceWebSettings(models.Model):
   device = models.OneToOneField(Device)
   utility_companies = models.ManyToManyField(UtilityCompany)
   rate_plans = models.ManyToManyField(RatePlan)
   territories = models.ManyToManyField(Territory)
   current_tier = models.ForeignKey(Tier)

class DashboardSettings(models.Model):
   user = models.OneToOneField(User)
   stack = models.BooleanField(
      default=True,
      help_text="Specifies the default behavior for a graph: stacked or unstacked line chart"
   )
