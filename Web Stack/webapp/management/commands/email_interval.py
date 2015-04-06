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

def render_chart(user):
   x = np.linspace(0, 10)
   with plt.style.context('fivethirtyeight'):
      plt.plot(x, np.sin(x) + x + np.random.randn(50))
      plt.plot(x, np.sin(x) + 0.5 * x + np.random.randn(50))
      plt.plot(x, np.sin(x) + 2 * x + np.random.randn(50))
   plt.savefig(settings.STATIC_PATH+'/webapp/img/'+user.username+'_plot.png')

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
                  render_chart(user)
                  template = Template(text)
                  context = Context({
                             'organization': settings.ORG_NAME,
                             'base_url': settings.BASE_URL,
                             'interval': interval,
                             'interval_lower': interval.lower(),
                             'user_firstname': user.first_name,
                             'plot_location': 'http://'+settings.BASE_URL+'/static/webapp/img/'+user.username+'_plot.png'
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
            