from django.core.management.base import BaseCommand, CommandError
from microdata.models import Device
from webapp.models import Tier, RatePlan


class Command(BaseCommand):
  help = ''
  args = 'daily, weekly'

  def handle(self, *args, **options):
    for device in Device.objects.all():
      if (args[0] == 'daily'):
        device.kilowatt_hours_daily = 0
      else:
        device.kilowatt_hours_monthly = 0
        rate_plan = device.devicewebsettings.rate_plans.all()[0]
        tiers = Tier.objects.filter(rate_plan=rate_plan)
        for tier in tiers:
        	if tier.tier_level == 1:
        		device.devicewebsettings.current_tier = tier
      device.save()