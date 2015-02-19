from django.shortcuts import render
from django.shortcuts import render_to_response
from django.db.models import Q
from django.contrib.auth.decorators import login_required
from microdata.models import Device, Event, Appliance
from django.http import HttpResponse
from django.conf import settings
from sets import Set
from collections import defaultdict
from urllib2 import Request, urlopen, URLError
from pyzipcode import ZipCodeDatabase

import datetime
import json
import time
import timeseries as ts
import re
from influxdb import client as influxdb

# Create your views here.

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
   user = request.user.id
   my_devices = Device.objects.filter(owner=user)
   context = {'my_devices': my_devices}
   return render(request, 'base/landing.html', context)


@login_required(login_url='/signin/')
def dashboard(request):
   user = request.user.id
   if request.user.is_authenticated():
      my_devices = Device.objects.filter(owner=user)
   else: my_devices = None
   device = my_devices[0] if my_devices else None
   result = []
   if (device != None):
      db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
      result = db.query('select * from "'+str(device.serial)+'" limit 1;')[0]
   context = {'my_devices': my_devices,
              'appliances': result['columns'][2:]
              }
   return render(request, 'base/dashboard.html', context)


def merge_subs(lst_of_lsts):
    res = []
    for row in lst_of_lsts:
        for i, resrow in enumerate(res):
            if row[0]==resrow[0]:
                res[i] += row[1:]
                break
        else:
            res.append(row)
    return res


def group_by_mean(serial, unit, start, stop):
   if (unit == 'y'): unit = 'm'
   if (start == ''): start = 'now() - 1d'
   else: start = '\''+datetime.datetime.fromtimestamp(int(start)/1000).strftime('%Y-%m-%d %H:%M:%S')+'\''
   if (stop == ''): stop = 'now()'
   else: stop = '\''+datetime.datetime.fromtimestamp(int(stop)/1000).strftime('%Y-%m-%d %H:%M:%S')+'\''
   db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
   result = db.query('list series')[0]
   appliances = Set()
   for series in result['points']:
      rg = re.compile('device.'+str(serial))
      if re.match(rg, series[1]):
         appliance = series[1].split('device.'+str(serial)+'.')
         if (len(appliance) < 2): continue
         else: appliances.add(appliance[-1])
   mean = {}
   to_merge = []
   for appliance in appliances:
      group = db.query('select * from 1'+unit+'.device.'+str(serial)+'.'+appliance+' where time > '+start+' and time < '+stop+' limit 1000')
      if (len(group)): group = group[0]['points']
      else: return None
      # hack. Remove sequence_number and timezone offset for GMT
      new_group = []
      for s in group:
         s = [s[0]-28800,s[2]]
         new_group.append(s)
      to_merge += new_group
   data = merge_subs(to_merge)
   for point in data:
      point.insert(1, sum(point[1:]))
   if (len(data) < 1): return None
   data = {'data': data,
           'unit': unit,
           'dataLimitFrom': data[len(data)-1][0],
           'dataLimitTo': data[0][0],
          }
   return data


@login_required(login_url='/signin/')
def default_chart(request):
   if request.method == 'GET':
      user = request.user.id
      device = Device.objects.filter(owner=user)[0]
      context = {}
      if (device):
         db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
         result = db.query('list series')[0]
         appliances = Set()
         for series in result['points']:
            rg = re.compile('device.'+str(device.serial))
            if re.match(rg, series[1]):
               appliance = series[1].split('device.'+str(device.serial)+'.')
               if (len(appliance) < 2): continue
               else: appliances.add(appliance[-1])
         context = {'my_devices': [device],
                    'appliances': appliances,
                    }
      return render(request, 'base/dashboard.html', context)


@login_required(login_url='/signin/')
def charts(request, serial):
   if request.method == 'GET':
      unit = request.GET.get_weather('unit','')
      start = request.GET.get('from','')
      stop = request.GET.get('to','')
      return HttpResponse(json.dumps(group_by_mean(serial,unit,start,stop)), content_type="application/json")


@login_required(login_url='/signin/')
def charts_deprecated(request, serial, unit):
   warnings.warn("Generating chart data from sqlite deprecated. See new charts() using InfluxDB.", DeprecationWarning)
   if request.method == 'GET':
      user = request.user.id
      devices = Device.objects.filter(owner=user, serial=serial)
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

def device_is_online(device):
  if device:
    db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
    result = db.query('select * from device.'+str(device.serial)+' limit 1;')[0]['points'][0]
    return int(time.time()) - int(result[0]) < 10


@login_required(login_url='/signin/')
def settings(request):
  context = {}
  user = request.user.id
  if request.GET.get('device', False):
    devices = Device.objects.filter(owner=user)
    for device in devices:
      device.online = device_is_online(device)
    context['devices'] = devices
    context['action'] = 'device'
    return render(request, 'base/settings_device.html', context)
  elif request.GET.get('account', False):
    context['user'] = user
    context['action'] = 'account'
    return render(request, 'base/settings_account.html', context)
  elif request.GET.get('dashboard', False):
    context['aciton'] = 'dashboard'
    return render(request, 'base/settings_dashboard.html', context)
  else:
    return render(request, 'base/settings.html', context)
      
