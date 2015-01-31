from django.shortcuts import render
from django.shortcuts import render_to_response
from django.db.models import Q
from django.contrib.auth.decorators import login_required
from microdata.models import Device, Event, Appliance
from django.http import HttpResponse
from django.conf import settings
import json
from sets import Set
from collections import defaultdict
from urllib2 import Request, urlopen, URLError
from pyzipcode import ZipCodeDatabase

import timeseries as ts

from influxdb import client as influxdb

# Create your views here.

#TODO O(n^2) time
def chartify(data):
   warnings.warn("chartify method no longer used for InfluxDB.", DeprecationWarning)
   values = []
   for time in data:
      tmp = [time]
      for value in data[time]:
         tmp.append(value)
      values.append(tmp)
   return values

@login_required(login_url='/signin/')
def landing(request):
   public_devices = Device.objects.filter(private=False)
   user = request.user.id
   my_devices = Device.objects.filter(owner=user)
   context = {'public_devices': public_devices, 'my_devices': my_devices}
   return render(request, 'base/landing.html', context)

@login_required(login_url='/signin/')
def dashboard(request):
   public_devices = Device.objects.filter(private=False)
   user = request.user.id
   if request.user.is_authenticated():
      my_devices = Device.objects.filter(owner=user)
   else: my_devices = public_devices
   device = my_devices[0] if my_devices else None
   result = []
   if (device != None):
      db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
      result = db.query('select * from "'+str(device.serial)+'" limit 1;')[0]
   context = {'public_devices': public_devices,
              'my_devices': my_devices,
              'appliances': result['columns'][2:]
              }
   return render(request, 'base/dashboard.html', context)

def second(events, appliances, values):
   for event in events:
      if (event.appliance == None):
         appliances.add("Unknown")
      else: appliances.add(event.appliance.name)
      values[event.timestamp].append(event.wattage)
   for time in values: values[time].insert(0,sum(values[time]))
   return [appliances, values]

units = {'s': second,
         'm': second,
         'h': second,
         'd': second,
         'M': second,
         'y': second,
}

def get_weather(zipcode):
   zcdb = ZipCodeDatabase()
   zipcode = zcdb[zipcode]
   city = zipcode.city.replace(" ", "_") + "," + zipcode.state + ",us"
   #url = "http://api.openweathermap.org/data/2.5/history?q="+city+"&type=hour&APPID="+settings.OWM_KEY
   #request = Request(url)
   #response = urlopen(request)
   #response = json.load(response)
   #return response

def charts(request, serial, unit):
   if request.method == 'GET':
      user = request.user.id
      devices = Device.objects.filter(Q(owner=user) | Q(private=False), serial=serial)
      device = devices[0] if devices else None
      public_devices = Device.objects.filter(private=False)
      user = request.user.id
      if request.user.is_authenticated():
         my_devices = Device.objects.filter(owner=user)
      else: my_devices = public_devices
      context = {}
      if (device):
         db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
         result = db.query('select * from "'+serial+'";')[0]
         for point in result['points']:
            point.pop(1)
            point[0] = int(point[0])
            point.insert(1, sum(point[1:]))
         appliances = result['columns'][2:]
         data = result['points']
         context = {'public_devices': public_devices,
                    'my_devices': my_devices,
                    'appliances': result['columns'][2:],
                    'data': json.dumps(data),
                    'unit': unit,
                    'dataLimitFrom': result['points'][len(result['points'])-1][0],
                    'dataLimitTo': result['points'][0][0]
                    }
      return render(request, 'base/dashboard.html', context)


#TODO request parameters
def charts_deprecated(request, serial, unit):
   warnings.warn("Generating chart data from sqlite deprecated. See new charts() using InfluxDB.", DeprecationWarning)
   if request.method == 'GET':
      user = request.user.id
      devices = Device.objects.filter(Q(owner=user) | Q(private=False), serial=serial)
      device = devices[0] if devices else None
      events = Event.objects.filter(device=device)
      response_data = {}
      response_data['unit'] = unit
      response_data['values'] = []
      if events:
         values = defaultdict(list)
         appliances = Set()
         [appliances, values] = units[unit](events, appliances, values)
         response_data['weather'] = get_weather(str(device.zipcode))
         response_data['appliances'] = list(appliances)
         response_data['values'] = chartify(values)
         response_data['dataLimitFrom'] = events[0].timestamp
         response_data['dataLimitTo'] = events[len(events)-1].timestamp
      return HttpResponse(json.dumps(response_data), content_type="application/json")
      
