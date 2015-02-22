from django.db import models
from django.contrib.auth.models import User

# Create your models here.

class Notification(models.Model):
   description = models.CharField(max_length=300)

   def __unicode__(self):
      return self.description

class UserSettings(models.Model):
   user = models.OneToOneField(User)
   notifications = models.ManyToManyField(Notification)