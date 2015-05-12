# Create your views here.
from django.http import HttpResponse, HttpResponseRedirect
from django.views.decorators.csrf import csrf_exempt
from microdata.models import Event, Device, Appliance, Circuit
from django import forms
from django.shortcuts import render, render_to_response
from django.forms import ModelChoiceField
from django.core import serializers

from influxdb.influxdb08 import client as influxdb
from gmapi import maps
from gmapi.maps import Geocoder
from gmapi.forms.widgets import GoogleMap
from django.contrib.gis.geoip import GeoIP

import git
import random
import json
import numpy
import re
import time
from datetime import datetime

from debug.models import TestEvent
from rest_framework import viewsets
from debug.serializers import TestEventSerializer
from webapp.models import Tier

class TestEventViewSet(viewsets.ModelViewSet):
    queryset = TestEvent.objects.all()
    serializer_class = TestEventSerializer

class DeviceModelChoiceField(ModelChoiceField):
   def label_from_instance(self, obj):
      return "%s (%i)" % (obj, obj.serial)

class DatagenForm(forms.Form):
   device = DeviceModelChoiceField(label='Device', queryset=Device.objects.all())
   appliances = forms.ModelMultipleChoiceField(label='Appliances', queryset=Appliance.objects.all())
   start = forms.IntegerField(label='Start (10-digit timestamp) UTC')
   stop = forms.IntegerField(label='Stop (10-digit timestamp) UTC')
   resolution = forms.IntegerField(label='Resolution (seconds)')

class DatadelForm(forms.Form):
   device = DeviceModelChoiceField(label='Device', queryset=Device.objects.all())
   refresh_queries = forms.BooleanField(label='Refresh Queries', required=False);

class DevForm(forms.Form):
   method = forms.ChoiceField(choices=(('datagen','datagen'),('datadel','datadel')))

class MapForm(forms.Form):
   map = forms.Field(widget=GoogleMap(attrs={'width':510,'height':510}))

@csrf_exempt
def position(request):
   lat = float(request.POST.get('lat', 0))
   lon = float(request.POST.get('lon', 0))
   gmap = maps.Map(opts = {
        'center':maps.LatLng(lat,lon),
        'mapTypeId':maps.MapTypeId.ROADMAP,
        'zoom':15,
        'mapTypeControlOptions':{
           'style': maps.MapTypeControlStyle.DROPDOWN_MENU
        },
   })
   marker = maps.Marker(opts = {
        'map': gmap,
        'position': maps.LatLng(lat,lon),
        'draggable':True,
        'title':"Drag me!"
   })
   context = {'form': MapForm(initial={'map':gmap})}
   return render_to_response('gmapi_form.html',context)

def gmapi(request):
   #g = GeoIP(path='/home/ubuntu/seads-git/ShR2/Web Stack/webapp/static/webapp/dat/')
   #ip = request.META.get('REMOTE_ADDR', None)
   #lat = 37
   #lon = -122
   #if ip:
   #   lat,lon = g.lat_lon(ip)
   #gmap = maps.Map(opts = {
   #     'center':maps.LatLng(lat,lon),'mapTypeId':maps.MapTypeId.ROADMAP,'zoom':9,'mapTypeControlOptions':{
   #        'style': maps.MapTypeControlStyle.DROPDOWN_MENU
   #     },
   #})
   #context = {'form': MapForm(initial={'map':gmap})}
   return render_to_response('gmapi2.html')#,context)

@csrf_exempt
def gitupdate(request):
    if request.method == 'POST':
        try:
            g = git.cmd.Git("/home/ubuntu/seads-git/ShR2/")
            g.pull()
            return HttpResponse(status=200)
        except: return HttpResponse(status=500)
    else: return HttpResponse(status=403)

@csrf_exempt
def echo(request):
   return HttpResponse(str(time.time()), status=200, content_type="text/plain")

@csrf_exempt
def echo_args(request, args):
   return HttpResponse(status=200)

@csrf_exempt
def influxgen(request):
   success = ""
   if request.method == 'POST':
      form = DatagenForm(request.POST)
      if form.is_valid():
         device = form.cleaned_data['device']
         appliances = form.cleaned_data['appliances']
         start = form.cleaned_data['start']
         stop = form.cleaned_data['stop']
         resolution = form.cleaned_data['resolution']
         circuits = [device.channel_0, device.channel_1, device.channel_2]
         wattages = {
            'Unknown':{
              'avg':700,
              'cutoff':0,
              'max':1500,
              'min':300,
              'circuit': [circuit for circuit in circuits if Appliance.objects.get(name='Computer') in circuit.appliances.all()]
            },
              'Computer':{
              'avg':200,
              'cutoff':50,
              'max':350,
              'min':0,
              'circuit': [circuit for circuit in circuits if Appliance.objects.get(name='Computer') in circuit.appliances.all()]
            },
              'Toaster':{
              'avg':20,
              'cutoff':0,
              'max':60,
              'min':0,
              'circuit': [circuit for circuit in circuits if Appliance.objects.get(name='Toaster') in circuit.appliances.all()]
            },
              'Refrigerator':{
              'avg':400,
              'cutoff':0,
              'max':600,
              'min':0,
              'circuit': [circuit for circuit in circuits if Appliance.objects.get(name='Refrigerator') in circuit.appliances.all()]
            },
              'Television':{
              'avg':100,
              'cutoff':50,
              'max':200,
              'min':0,
              'circuit': [circuit for circuit in circuits if Appliance.objects.get(name='Television') in circuit.appliances.all()]
            },
              'Oven':{
              'avg':700,
              'cutoff':600,
              'max':1000,
              'min':0,
              'circuit': [circuit for circuit in circuits if Appliance.objects.get(name='Oven') in circuit.appliances.all()]
            },
              'Heater':{
              'avg':8000,
              'cutoff':600,
              'max':1000,
              'min':0,
              'circuit': [circuit for circuit in circuits if Appliance.objects.get(name='Heater') in circuit.appliances.all()]
            },
          }
         count = 0
         data = []
         data_dict = {}
         data_dict['name'] = "device."+str(device.serial)
         data_dict['columns'] = ['time','appliance','wattage','circuit','cost']
         data_dict['points'] = []

         this_year = datetime.now().year
         summer_start = datetime(year=this_year,month=device.devicewebsettings.territories.all()[0].summer_start,day=1)
         winter_start = datetime(year=this_year,month=device.devicewebsettings.territories.all()[0].winter_start,day=1)
         current_season = 'summer'
         if (summer_start <= datetime.now() < winter_start) == False:
            current_season = 'winter'

         prev_i = start-resolution
         for i in numpy.arange(start, stop, resolution):
            point_list = [i]
            for appliance in appliances:
               wattage = wattages[appliance.name]['avg'] + random.uniform(-wattages[appliance.name]['avg']*0.1,wattages[appliance.name]['avg']*0.1)
               kwh = (wattage/1000.0)*(i - prev_i)*(1/3600.0)
               device.kilowatt_hours_monthly += kwh
               device.kilowatt_hours_daily += kwh
               if (device.devicewebsettings.current_tier.max_percentage_of_baseline != None):
                max_kwh_for_tier = (device.devicewebsettings.current_tier.max_percentage_of_baseline/100.0)*device.devicewebsettings.territories.all()[0].summer_rate
                if current_season == 'winter':
                   max_kwh_for_tier = (device.devicewebsettings.current_tier.max_percentage_of_baseline/100.0)*device.devicewebsettings.territories.all()[0].winter_rate
                if (device.kilowatt_hours_monthly > max_kwh_for_tier):
                   current_tier = device.devicewebsettings.current_tier
                   device.devicewebsettings.current_tier = Tier.objects.get(tier_level=(current_tier.tier_level + 1))
               cost = device.devicewebsettings.current_tier.rate * kwh
               prev_i = i
               wattage_to_append = 0
               if wattage > wattages[appliance.name]['max']:
                  wattage_to_append = wattages[appliance.name]['max']
               elif wattage < wattages[appliance.name]['cutoff']:
                  wattage_to_append = 0
               elif wattage < wattages[appliance.name]['min']:
                  wattage_to_append = wattages[appliance.name]['min']
               else:
                  wattages[appliance.name]['avg'] = wattage
                  wattage_to_append = wattage
               circuit = wattages[appliance.name]['circuit'] or [Circuit.objects.get(name='Unknown')]
               circuit_pk = circuit[0].pk
               point_list = [i, appliance.name, wattage_to_append, circuit_pk, cost]
               count += 1
               data_dict['points'].append(point_list)
         data.append(data_dict)
         db = influxdb.InfluxDBClient("localhost", 8086, "root", "root", "seads")
         if (db.write_points(data)):
            success = "Added {0} points successfully".format(count)
         device.save()
   else:
      form = DatagenForm()
   title = "Debug - Data Generation (InfluxDB)"
   description = "Use this form to submit random generated data for the device chosen."
   return render(request, 'debug.html', {'title':title,'description':description,'form':form, 'success':success})

def datagen(request):
   success = ""
   if request.method == 'POST':
      form = DatagenForm(request.POST)
      if form.is_valid():
         device = form.cleaned_data['device']
         appliances = form.cleaned_data['appliances']
         start = form.cleaned_data['start']
         stop = form.cleaned_data['stop']
         resolution = form.cleaned_data['resolution']
         wattages = {'Unknown':{'avg':700, 'stdev':20}, 'Computer':{'avg':100, 'stdev':50}, 'Toaster':{'avg':20, 'stdev':20}, 'Refrigerator':{'avg':400,'stdev':200}, 'Television':{'avg':60,'stdev':60}}
         count = 0
         for i in numpy.arange(start, stop, resolution):
             for appliance in appliances:
                 wattage = wattages[appliance.name]['avg'] + random.uniform(-wattages[appliance.name]['stdev'],wattages[appliance.name]['stdev'])
                 event = Event(device=device, timestamp=i*1000, wattage=wattage, appliance=appliance)
                 event.save()
                 count += 1
         success = "Added {0} events successfully".format(count)
   else:
      form = DatagenForm()
   title = "Debug - Data Generation"
   description = "Use this form to submit random generated data for the device chosen."
   return render(request, 'debug.html', {'title':title,'description':description,'form':form, 'success':success})

@csrf_exempt
def influxdel(request):
   success = ""
   if request.method == 'POST':
      form = DatadelForm(request.POST)
      count = 0
      if form.is_valid():
         refresh_queries = form.cleaned_data['refresh_queries']
         device = form.cleaned_data['device']
         serial = str(device.serial)
         db = influxdb.InfluxDBClient("localhost", 8086, "root", "root", "seads")
         if refresh_queries is False:
           device.kilowatt_hours_monthly = 0
           device.kilowatt_hours_daily = 0
           device.save()
           series = db.query('list series')[0]['points']
           rg = re.compile('device.'+serial)
           for s in series:
              if rg.search(s[1]):
                 db.query('drop series "'+s[1]+'"')
           events = Event.objects.filter(device=device)
           events.delete()
         else:
           queries = db.query('list continuous queries')[0]['points']
           # drop old queries
           for q in queries:
             if 'device.'+serial in q[2]:
               db.query('drop continuous query '+str(q[1]))
           # add new queries
           db.query('select mean(wattage) from /^device.'+serial+'.*/ group by time(1y) into 1y.:series_name')
           db.query('select mean(wattage) from /^device.'+serial+'.*/ group by time(1M) into 1M.:series_name')
           db.query('select mean(wattage) from /^device.'+serial+'.*/ group by time(1w) into 1w.:series_name')
           db.query('select mean(wattage) from /^device.'+serial+'.*/ group by time(1d) into 1d.:series_name')
           db.query('select mean(wattage) from /^device.'+serial+'.*/ group by time(1h) into 1h.:series_name')
           db.query('select mean(wattage) from /^device.'+serial+'.*/ group by time(1m) into 1m.:series_name')
           db.query('select mean(wattage) from /^device.'+serial+'.*/ group by time(1s) into 1s.:series_name')
           db.query('select * from device.'+serial+' into device.'+serial+'.[appliance]')
   else:
      form = DatadelForm()
   title = "Debug - Data Deletion"
   description = "Use this form to delete data for the device chosen."
   return render(request, 'debug.html', {'title':title,'description':description,'form':form, 'success':success})
   

def datadel(request):
   success = ""
   if request.method == 'POST':
      form = DatadelForm(request.POST)
      count = 0
      if form.is_valid():
         device = form.cleaned_data['device']
         events = Event.objects.filter(device=device)
         count = len(events)
         events.delete()
         success = "Deleted {0} events successfully".format(count)
   else:
      form = DatadelForm()
   title = "Debug - Data Deletion"
   description = "Use this form to delete data for the device chosen."
   return render(request, 'debug.html', {'title':title,'description':description,'form':form, 'success':success})
