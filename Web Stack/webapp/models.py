from django.db import models
from django.contrib.auth.models import User
from microdata.models import Device, Appliance
from django.conf import settings
from recurrence.fields import RecurrenceField
from paintstore.fields import ColorPickerField

# Create your models here.

class EventNotification(models.Model):
   """
   Notification sent to users via email whenever a notable event is detected.

   This class is not currently in use since the system is not set up in such
   a way as to detect any events. However, the notification framework is in
   place such that when the functionality is added, this class should be called
   in response to an event.

   These notifications can be added/modified via the admin interface.
   """

   description = models.CharField(
      max_length=300,
      help_text="Label to notification as shown to a user"
   )
   """ The description of the event notification as a user would see it when selecting/deselecting the notification in the settings interface"""

   keyword = models.CharField(
      max_length=300,
      help_text="Keyword used to launch manage.py email_event",
   )
   """ Used to trigger the event notification in the django manager."""

   watts_above_average = models.FloatField()
   """ Proof of concept field to provide a threshold. If a group of appliances surpasses the threshold for a period of time, then send the email."""

   period_of_time = models.FloatField(
      help_text="Period of time to watch for irregularity"
   )
   """ Proof of concept field to provide a threshold. If a group of appliances surpasses the threshold for a period of time, then send the email."""

   appliances_to_watch = models.ManyToManyField(Appliance)
   """ Assemble a group of appliances to watch. Could be one or many."""

   email_subject = models.CharField(max_length=300)
   """ An email-friendly subject for the event notification."""

   email_body = models.FileField(
      help_text="Template file for email body. {{ x }} denotes x is a template variable",
      upload_to="event"
   )
   """ A template used to generate the notification email body."""
   
   def __unicode__(self):
      return self.description

class IntervalNotification(models.Model):
   """
   Notifications sent to users when a specified period has elapsed.

   This class is also proof-of-concept, and it relies upon the `Amazon Simple Email Service <http://aws.amazon.com/ses/>`_.
   An email will be sent to users who opt in to the notification summarizing their devices' energy usage over the specified
   period.
   """

   description = models.CharField(
      max_length=300,
      help_text="Label to notification as shown to a user",
   )
   """ The description of the event notification as a user would see it when selecting/deselecting the notification in the settings interface"""

   recurrences = RecurrenceField(blank=True, null=True)
   """ This field is treated much like a Google Calendars recurrence field. Provides an easy way for an admin to define new periods of time."""

   email_subject = models.CharField(max_length=300)
   """ An email-friendly subject for the event notification."""

   email_body = models.FileField(
      help_text="Template file for email body. {{ x }} denotes template variable",
      upload_to="interval"
   )
   """ A template used to generate the notification email body."""

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
   """
   An encapsulating module that links a user's settings together.

   This model can be extended to include new settings that may come to be in the future.
   """

   user = models.OneToOneField(User)
   """ The related model for a settings model."""

   interval_notification = models.ManyToManyField(IntervalNotification, blank=True)
   """ A list of interval notifications that the user has opted in to. Default to none."""

   event_notification = models.ManyToManyField(EventNotification, blank=True)
   """ A list of event notifications that the user has opted in to. Default to none."""


class UtilityCompany(models.Model):
   """
   A placeholder class to describe a Utility Company.

   Since PG&E is the only company that was developed on during the proof-of-concept phase,
   it is the company that was used to model the pricing structures. In the future, in order
   to integrate new types of companies, a Utility Company model should reflect how the Utility
   Company calculates cost.
   """

   description = models.CharField(max_length=300)
   """ A label that describes what company this is. Used for selection."""

   class Meta:
        verbose_name_plural = "Utility Companies"

   def __unicode__(self):
      return self.description


class RatePlan(models.Model):
   """
   The base class that describes how a user is charged in the Utility Company.

   This class is linked to :class:`webapp.models.UtilityCompany` via a ForeignKey.
   In addition, the class contains a list of :class:`webapp.models.Tier` objects
   that describe how the charges change based on usage.
   """

   utility_company = models.ForeignKey(UtilityCompany)
   """ Utility company relation. Describe who owns the :class:`webapp.models.RatePlan`"""

   description = models.CharField(max_length=300)
   """ A short description for the user when selecting their :class:`webapp.models.RatePlan`."""

   data_source = models.URLField()
   """ A simple URL field that links to the source of the data for this :class:`webapp.models.RatePlan`."""

   min_charge_rate = models.FloatField(help_text="$ Per meter per day")
   """ The minimum amount charged to a user's account. Not currently in use."""

   california_climate_credit = models.FloatField(help_text="$ Per household, per semi-annual payment occurring in the April and October bill cycles")
   """ A credit applied to a user's account twice yearly. Not currently in use."""


   def __unicode__(self):
      return self.utility_company.__unicode__() + ": " + self.description

class Tier(models.Model):
   """
   A class that defines the cost and threshold of a :class:`webapp.models.RatePlan`.

   A :class:`webapp.models.RatePlan` typically has 4-5 :class:`webapp.models.Tier` objects
   as a relation. These objects keep track of the cost modifier as well as the KWh threshold
   for a given device.
   """

   rate_plan = models.ForeignKey(RatePlan)
   """ This object is related to a :class:`webapp.models.RatePlan`."""

   tier_level = models.IntegerField(blank=True, null=True)
   """ An Integer, starting at 1, indicating the current level of the device."""

   max_percentage_of_baseline = models.FloatField(help_text="blank for no maximum",blank=True, null=True)
   """ This defines the threshold for a given :class:`webapp.models.Tier`. I.e. 100% - 130%"""

   rate = models.FloatField(help_text="$",blank=True, null=True)
   """ The actual cost of a KWh at this level."""

   chart_color = ColorPickerField()
   """ Color used by charts when graphing a :class:`webapp.models.Tier`."""

   def __unicode__(self):
      return 'Tier ' + str(self.tier_level)

class Territory(models.Model):
   """
   A :class:`webapp.models.Territory` defines specifically key fields associated with a :class:`webapp.models.RatePlan`.

   This class specifies the base rates of a given :class:`webapp.models.RatePlan` as well as defining
   the winter and summer seasons for seasonal pricing.
   """

   rate_plan = models.ForeignKey(RatePlan)
   """ This object is related to a :class:`webapp.models.RatePlan`."""

   description = models.CharField(max_length=300)
   """ A short description for the user when selecting their :class:`webapp.models.RatePlan`."""

   data_source = models.URLField()
   """ A simple URL field that links to the source of the data for this :class:`webapp.models.RatePlan`."""

   summer_start = models.IntegerField(blank=True,null=True,help_text="Specify Month of year")
   """ A month of the year that specifies the start of summer. 1-12."""

   winter_start = models.IntegerField(blank=True,null=True,help_text="Specify Month of year")
   """ A month of the year that specifies the start of winter. 1-12."""

   summer_rate = models.FloatField(help_text="Baseline quantity (kWh per day)")
   """ The base rate for the summer season."""

   winter_rate = models.FloatField(help_text="Baseline quantity (kWh per day)")
   """ The base rate for the winter season."""

   class Meta:
        verbose_name_plural = "Territories"

   def __unicode__(self):
      return self.description


class DeviceWebSettings(models.Model):
   """
   An encapsulating module that links a device's settings together.

   This model can be extended to include new settings that may come to be in the future.
   """

   device = models.OneToOneField(Device)
   utility_companies = models.ManyToManyField(UtilityCompany, default=[1])
   rate_plans = models.ManyToManyField(RatePlan, default=[1])
   territories = models.ManyToManyField(Territory, default=[1])
   current_tier = models.ForeignKey(Tier, default=1)

class DashboardSettings(models.Model):
   user = models.OneToOneField(User)
   stack = models.BooleanField(
      default=True,
      help_text="Specifies the default behavior for a graph: stacked or unstacked line chart"
   )
