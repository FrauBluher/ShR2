from django.core.management.base import BaseCommand, CommandError
from django.contrib.auth.models import User
from webapp.models import Notification
from django.core.exceptions import ObjectDoesNotExist
import boto3
from botocore.exceptions import ClientError

class Command(BaseCommand):
   args = '<weekly, monthly>'
   help = 'Launches the mail service to send usage information based on the provided interval'
   
   def handle(self, *args, **options):
      intervals = {'weekly': {'alt-text': 'Weekly', 'pk': 15}, 'monthly': {'alt-text': 'Monthly', 'pk':13}}
      # http://seads.brabsmit.com/admin/webapp/notification/
      if intervals.get(args[0] == None): raise CommandError('Interval "%s" does not exist' % arg)
      ses = boto3.client('ses')
      for user in User.objects.all():
         try:
            notifications = user.usersettings.notifications.all()
            for notification in notifications:
               if notification.pk == intervals[args[0]]['pk']:
                  # current user requests the given interval
                  destination = {'ToAddresses': [user.email]}
                  
                  html = """\
                  
                  """
                  
                  message = {
                     'Subject': {
                        'Data': 'SEADS ' + intervals[args[0]]['alt-text'] + ' Consumption Details'
                     },
                     'Body': {
                        'Html': {
                           'Data': html
                        }
                     }
                  }
                  ses.send_email(Source='seadsystems@gmail.com',
                                 Destination=destination,
                                 Message=message)
         except ObjectDoesNotExist:
            # user has no usersettings. Skip user.
            pass
         except ClientError:
            # user has no email or is not verified. Skip for now.
            pass
            