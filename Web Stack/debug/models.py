from django.db import models
from microdata.models import Appliance, Device
from influxdb.influxdb08 import client as influxdb
import json

# Create your models here.


class TestEvent(models.Model):
	device = models.ForeignKey(Device)
	dataPoints = models.CharField(max_length=1000,help_text='Expects a JSON encoded string of values: [{timestamp(int), wattage(float, optional)},...]')

	def save(self, **kwargs):
		db = influxdb.InfluxDBClient(settings.INFLUXDB_URI,8086,'root','root','seads')
		data = []
		query = {}
		query['points'] = []
		for point in json.loads(self.dataPoints):
			query['points'].append([point['timestamp'], point['wattage']])
		query['name'] = 'zz_debug'
		query['columns'] = ['time', 'wattage']
		data.append(query)
		db.write_points(data)

		super(TestEvent, self).save()
   

