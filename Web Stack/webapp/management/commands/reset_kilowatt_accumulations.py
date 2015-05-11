from django.core.management.base import BaseCommand, CommandError
from microdata.models import Device


class Command(BaseCommand):
  help = ''
  args = ''

  def handle(self, *args, **options):
    for device in Device.objects.all():
      device.kilowatt_hours_monthly = 0
      device.save()