from django.shortcuts import render
from django.shortcuts import render_to_response
from django.db.models import Q
from microdata.models import Device, Event, Appliance
from django.http import HttpResponse
import json
from sets import Set
from collections import defaultdict

import timeseries as ts

# Create your views here.

#TODO O(n^2) time
def chartify(data):
   values = []
   for time in data:
      tmp = [time]
      for value in data[time]:
         tmp.append(value)
      values.append(tmp)
   return values

def landing(request):
   public_devices = Device.objects.filter(private=False)
   user = request.user.id
   my_devices = Device.objects.filter(owner=user)
   context = {'public_devices': public_devices, 'my_devices': my_devices}
   return render(request, 'base/landing.html', context)

def dashboard(request):
   public_devices = Device.objects.filter(private=False)
   user = request.user.id
   if request.user.is_authenticated():
      my_devices = Device.objects.filter(owner=user)
   else: my_devices = public_devices
   device = my_devices[0] if my_devices else None
   events = Event.objects.filter(device=device)
   appliances = Set()
   for event in events: appliances.add(event.appliance)
   context = {'public_devices': public_devices,
              'my_devices': my_devices,
              'appliances': list(appliances)
              }
   return render(request, 'base/dashboard.html', context)

def second(events, appliances, values):
   for event in events:
      appliances.add(event.appliance.name if event.appliance is not None else "None")
      values[event.timestamp].append(event.wattage)
   for time in values: values[time].insert(0,sum(values[time]))
   return [appliances, values]

def minute(events, appliances, values):
   for event in events:
      appliances.add(event.appliance.name if event.appliance is not None else "None")
      values[event.timestamp].append(event.wattage)
   for time in values: values[time].insert(0,sum(values[time]))
   return [appliances, values]
   
def hour(events, appliances, values):
   for event in events:
      appliances.add(event.appliance.name if event.appliance is not None else "None")
      values[event.timestamp].append(event.wattage)
   for time in values: values[time].insert(0,sum(values[time]))
   return [appliances, values]
   
def day(events, appliances, values):
   for event in events:
      appliances.add(event.appliance.name if event.appliance is not None else "None")
      values[event.timestamp].append(event.wattage)
   for time in values: values[time].insert(0,sum(values[time]))
   return [appliances, values]
   
def month(events, appliances, values):
   for event in events:
      appliances.add(event.appliance.name if event.appliance is not None else "None")
      values[event.timestamp].append(event.wattage)
   for time in values: values[time].insert(0,sum(values[time]))
   return [appliances, values]
   
def year(events, appliances, values):
   for event in events:
      appliances.add(event.appliance.name if event.appliance is not None else "None")
      values[event.timestamp].append(event.wattage)
   for time in values: values[time].insert(0,sum(values[time]))
   return [appliances, values]

units = {'s': second,
         'm': minute,
         'h': hour,
         'd': day,
         'M': month,
         'y': year,
}

#TODO request parameters
def charts(request, serial, unit):
   if request.method == 'GET':
      user = request.user.id
      device = Device.objects.filter(Q(owner=user) | Q(private=False), serial=serial)
      events = Event.objects.filter(device=device)
      response_data = {}
      response_data['unit'] = unit
      response_data['values'] = []
      if events:
         values = defaultdict(list)
         appliances = Set()
         [appliances, values] = units[unit](events, appliances, values)
         response_data['appliances'] = list(appliances)
         response_data['values'] = chartify(values)
         response_data['dataLimitFrom'] = events[0].timestamp
         response_data['dataLimitTo'] = events[len(events)-1].timestamp
      return HttpResponse(json.dumps(response_data), content_type="application/json")
      