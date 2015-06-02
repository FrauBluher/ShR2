from django.core.management.base import BaseCommand, CommandError
from django.contrib.auth.models import User
from webapp.models import Notification
from django.core.exceptions import ObjectDoesNotExist
import boto3
from botocore.exceptions import ClientError
from django.conf import settings
from matplotlib import pyplot as plt
import numpy as np
from django.template import Context, Template
import os
from time import gmtime, strftime
from influxdb.influxdb08 import client as influxdb
from microdata.models import Device
from sets import Set
import re
import datetime
from calendar import monthrange
import random
from math import factorial
from webapp.models import EventNotification


def render_chart(user, notification):
   date_today = datetime.datetime.today()
   date_gmtime = gmtime()
   randbits = str(random.getrandbits(128))
   start = 'now() - 1w'
   unit = 'h'
   if notification.keyword == 'monthly':
      start = 'now() - 1M'
      unit = 'd'
   elif notification.keyword == 'daily':
      start = 'now() - 1d'
      unit = 'm'
      
   stop = 'now()'
   db = influxdb.InfluxDBClient(settings.INFLUXDB_URI,8086,'root','root','seads')
   fig = plt.figure(figsize=(10, 5), dpi=100) # 1000px * 500px figure
   plt.ylabel('Watts')
   for device in Device.objects.filter(owner=user):
      points = {}
      result = db.query('list series')[0]
      appliances = Set()
      for series in result['points']:
         rg = re.compile('device.'+str(device.serial))
         if re.match(rg, series[1]):
            appliance = series[1].split('device.'+str(device.serial)+'.')
            if (len(appliance) < 2): continue
            else: appliances.add(appliance[-1])
      for appliance in appliances:
         query = 'select * from 1'+unit+'.device.'+str(device.serial)+'.'+appliance+' where time > '+start+' and time < '+stop
         try:
            group = db.query(query)
         except:
            continue
         if (len(group)): group = group[0]['points']
         for s in group:
            if s[0] in points:
               points[s[0]] += s[2]
            else:
               points[s[0]] = s[2]
      y = []
      for key, value in points.iteritems():
         y.append(value)
      x = 0
      if notification.keyword == 'monthly':
         x = np.array([date_today - datetime.timedelta(days=i) for i in range(len(y))])
      elif notification.keyword == 'weekly':
         x = np.array([date_today - datetime.timedelta(hours=i) for i in range(len(y))])
      elif notification.keyword == 'daily':
         x = np.array([date_today - datetime.timedelta(minutes=i) for i in range(len(y))])
      if (len(y) > 0):
         plt.plot(x, y, label=device)
   plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3,
           ncol=2, mode="expand", borderaxespad=0.)
   filepath = settings.STATIC_PATH+'/webapp/img/'
   filename = notification.keyword + '_' + str(user.pk)+'_'+randbits+'_plot.png'
   plt.savefig(filepath + filename, bbox_inches="tight")
   s3 = boto3.resource('s3')
   data = open(filepath + filename, 'rb')
   bucket = s3.Bucket(settings.S3_BUCKET)
   expires = datetime.datetime.today() + datetime.timedelta(days=90)
   bucket.put_object(Key='email/'+filename, Body=data, ACL='public-read', Expires=str(expires))
   resource_url = 'https://'+settings.S3_BUCKET+'.s3.amazonaws.com/email/'+filename
   os.remove(filepath + filename)
   return [resource_url, strftime("%a, %d %b %Y %H:%M:%S +0000", date_gmtime)]
         

class Command(BaseCommand):
   event_notifications = EventNotification.objects.all()
   args = '<'
   for en in event_notifications:
      args += en.keyword + ', '
   args += '>'
   help = 'Launches the mail service to send event information based on type of event triggered'

   def handle(self, *args, **options):
      ses = boto3.client('ses')
      for user in User.objects.all():
         try:
            notifications = user.usersettings.interval_notification.all()
            for notification in notifications:
               if notification.keyword == args[0]:
                  # current user requests the given interval
                  destination = {'ToAddresses': [user.email]}
                  text = ""
                  f = notification.email_body
                  f.open(mode='r')
                  text = f.read()
                  f.close()
                  plot_url, str_time = render_chart(user, notification)
                  average_objects = []
                  averages = get_average_usage(user, notification)
                  for key, value in averages.iteritems():
                     average_objects.append(Object(Device.objects.get(serial=key), value[0], value[1]))
                  template = Template(text)
                  context = Context({
                             'time': str_time,
                             'organization': settings.ORG_NAME,
                             'base_url': settings.BASE_URL,
                             'user_firstname': user.first_name,
                             'plot_location': plot_url,
                             'average_objects': average_objects,
                             'devices': Device.objects.filter(owner=user),
                           })
                  message = {
                     'Subject': {
                        'Data': settings.ORG_NAME + ' Irregular Appliance Usage Detected'
                     },
                     'Body': {
                        'Html': {
                           'Data': template.render(context)
                        }
                     }
                  }
                  ses.send_email(Source=settings.SES_EMAIL,
                                 Destination=destination,
                                 Message=message,
                                 ReturnPath=settings.SES_EMAIL
                  )
         except ObjectDoesNotExist:
            # user has no usersettings. Skip user.
            pass
         except ClientError:
            # user has no email or is not verified. Skip for now.
            pass
            