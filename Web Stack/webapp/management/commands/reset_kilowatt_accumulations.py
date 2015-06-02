from django.core.management.base import BaseCommand, CommandError
from microdata.models import Device
from webapp.models import Tier, RatePlan
from influxdb.influxdb08 import client as influxdb
from django.conf import settings


class Command(BaseCommand):
   help = ''
   args = 'daily, weekly'

   def handle(self, *args, **options):
      for device in Device.objects.all():
         if (args[0] == 'daily'):
            device.kilowatt_hours_daily = 0
            device.cost_daily = 0
         else:
            db = influxdb.InfluxDBClient(settings.INFLUXDB_URI, 8086, "root", "root", "seads")
            tier_dict = {}
            tier_dict['name'] = "tier.device."+str(device.serial)
            tier_dict['columns'] = ['tier_level']
            tier_dict['points'] = {"tier_level":1}
            db.write_points([tier_dict])
            device.kilowatt_hours_monthly = 0
            rate_plan = device.devicewebsettings.rate_plans.all()[0]
            tiers = Tier.objects.filter(rate_plan=rate_plan)
            device.devicewebsettings.current_tier = tiers.get(tier_level=1)
            device.devicewebsettings.save()
         device.save()
         with open('/home/ubuntu/reset.log','a') as f:
            f.write('Reset '+device.__unicode__())
            f.write('\n')