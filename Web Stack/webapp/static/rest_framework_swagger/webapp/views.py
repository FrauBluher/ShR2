from django.shortcuts import render

from django.shortcuts import render_to_response
from django.db.models import Q
from microdata.models import Device, Event
from django.http import HttpResponse
import json

# Create your views here.

def landing(request):
   public_devices = Device.objects.filter(private=False)
   user = request.user.id
   my_devices = Device.objects.filter(owner=user)
   context = {'public_devices': public_devices, 'my_devices': my_devices}
   return render(request, 'base/landing.html', context)


def devices(request, serial):
#TODO api parameters (boundaries)
   if request.method == 'GET':
      user = request.user.id
      device = Device.objects.filter(Q(owner=user) | Q(private=False), serial=serial)
      error_403 = False
      if (len(device) > 0): device = device[0]
      else: error_403 = True
      events = Event.objects.filter(device=device)
      context = {'device': device, 'event_array': events, 'error_403': error_403}
      return render_to_response('base/linechart.html', context)

def dashboard(request):
   public_devices = Device.objects.filter(private=False)
   user = request.user.id
   my_devices = Device.objects.filter(owner=user)
   context = {'public_devices': public_devices, 'my_devices': my_devices}
   return render(request, 'base/dashboard.html', context)

def charts(request, serial):
   if request.method == 'GET':
      user = request.user.id
      device = Device.objects.filter(Q(owner=user) | Q(private=False), serial=serial)
      events = Event.objects.filter(device=device)
      response_data = {}
      values = []
      response_data['unit'] = 's'
      for event in events:
         values.append([event.timestamp*1000, event.wattage])
      response_data['values'] = values
      response_data['dataLimitFrom'] = events[0].timestamp*1000
      response_data['dataLimitTo'] = events[len(events)-1].timestamp*1000
      return HttpResponse(json.dumps(response_data), content_type="application/json")


