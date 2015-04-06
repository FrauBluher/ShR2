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
from influxdb import client as influxdb
from microdata.models import Device
from sets import Set
import re
import datetime
import numpy as np
from calendar import monthrange
import random

# This command will generate a figure in the STATIC_PATH for every user.
   
def render_chart(user, interval):
   date_today = datetime.datetime.today()
   date_gmtime = gmtime()
   randbits = str(random.getrandbits(128))
   start = 'now() - 1w'
   unit = 'h'
   if interval['pk'] == 13:
      start = 'now() - 1M'
      unit = 'd'
      
   stop = 'now()'
   db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
   fig = plt.figure(figsize=(10, 5), dpi=100)
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
         group = db.query(query)
         if (len(group)): group = group[0]['points']
         for s in group:
            if s[0] in points:
               points[s[0]] += s[2]
            else:
               points[s[0]] = s[2]
      y = []
      for key, value in points.iteritems():
         y.append(value)
      x = np.array([date_today - datetime.timedelta(hours=i) for i in range(len(y))])
      if interval['pk'] == 13:
         x = np.array([date_today - datetime.timedelta(days=i) for i in range(len(y))])
      plt.plot(x, y)
   plt.savefig(settings.STATIC_PATH+'/webapp/img/' + interval['alt-text'] + '_' + str(user.pk)+'_'+randbits+'_plot.png')
   return [randbits, strftime("%a, %d %b %Y %H:%M:%S +0000", date_gmtime)]
         

class Command(BaseCommand):
   args = '<weekly, monthly>'
   help = 'Launches the mail service to send usage information based on the provided interval'
   
   def handle(self, *args, **options):
      intervals = {'weekly': {'alt-text': 'Weekly', 'pk': 15}, 'monthly': {'alt-text': 'Monthly', 'pk':13}}
      # http://seads.brabsmit.com/admin/webapp/notification/
      if intervals.get(args[0] == None): raise CommandError('Interval "%s" does not exist' % arg)
      interval = intervals[args[0]]['alt-text']
      ses = boto3.client('ses')
      for user in User.objects.all():
         try:
            notifications = user.usersettings.notifications.all()
            for notification in notifications:
               if notification.pk == intervals[args[0]]['pk']:
                  # current user requests the given interval
                  destination = {'ToAddresses': [user.email]}
                  text = ""
                  with open(settings.STATIC_PATH+"/webapp/email/consumption_details.txt", "r") as f:
                     text = f.read()
                  randbits, str_time = render_chart(user, intervals[args[0]])
                  template = Template(text)
                  context = Context({
                             'time': str_time,
                             'organization': settings.ORG_NAME,
                             'base_url': settings.BASE_URL,
                             'interval': interval,
                             'interval_lower': interval.lower(),
                             'period': interval.lower()[:-2],
                             'user_firstname': user.first_name,
                             'plot_location': 'http://'+settings.BASE_URL+'/static/webapp/img/'+ intervals[args[0]]['alt-text'] + '_' + str(user.pk)+'_'+randbits+'_plot.png'
                           })
                  message = {
                     'Subject': {
                        'Data': 'SEADS ' + intervals[args[0]]['alt-text'] + ' Consumption Details'
                     },
                     'Body': {
                        'Html': {
                           'Data': template.render(context)
                        }
                     }
                  }
                  ses.send_email(Source=settings.SES_EMAIL,
                                 Destination=destination,
                                 Message=message)
         except ObjectDoesNotExist:
            # user has no usersettings. Skip user.
            pass
         except ClientError:
            # user has no email or is not verified. Skip for now.
            pass
            