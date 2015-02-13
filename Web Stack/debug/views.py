# Create your views here.
from django.http import HttpResponse, HttpResponseRedirect
from django.views.decorators.csrf import csrf_exempt
from microdata.models import Event, Device, Appliance
from django import forms
from django.shortcuts import render, render_to_response
from django.forms import ModelChoiceField

from influxdb import client as influxdb
from gmapi import maps
from gmapi.forms.widgets import GoogleMap

import git
import random
import json
import numpy
import re

class DeviceModelChoiceField(ModelChoiceField):
   def label_from_instance(self, obj):
      return "%s (%i)" % (obj, obj.serial)

class DatagenForm(forms.Form):
   device = DeviceModelChoiceField(label='Device', queryset=Device.objects.all())
   appliances = forms.ModelMultipleChoiceField(label='Appliances', queryset=Appliance.objects.all())
   start = forms.IntegerField(label='Start (10-digit timestamp)')
   stop = forms.IntegerField(label='Stop (10-digit timestamp)')
   resolution = forms.IntegerField(label='Resolution (seconds)')

class DatadelForm(forms.Form):
   device = DeviceModelChoiceField(label='Device', queryset=Device.objects.all())

class DevForm(forms.Form):
   method = forms.ChoiceField(choices=(('datagen','datagen'),('datadel','datadel')))

class MapForm(forms.Form):
   map = forms.Field(widget=GoogleMap(attrs={'width':510,'height':510}))

def gmapi(request):
   gmap = maps.Map(opts = {
        'center':maps.LatLng(38,-97),'mapTypeId':maps.MapTypeId.ROADMAP,'zoom':3,'mapTypeControlOptions':{
           'style': maps.MapTypeControlStyle.DROPDOWN_MENU
        },
   })
   context = {'form': MapForm(initial={'map':gmap})}
   return render_to_response('gmapi.html',context)

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
   return HttpResponse(status=200)

@csrf_exempt
def echo_args(request, args):
   return HttpResponse(status=200)

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
         wattages = {'Unknown':{'avg':700, 'stdev':20}, 'Computer':{'avg':100, 'stdev':50}, 'Toaster':{'avg':20, 'stdev':20}, 'Refrigerator':{'avg':400,'stdev':200}, 'Television':{'avg':60,'stdev':60}}
         count = 0
         data = []
         data_dict = {}
         data_dict['name'] = "device."+str(device.serial)
         data_dict['columns'] = ['time','appliance','wattage']
         data_dict['points'] = []
         for i in numpy.arange(start, stop, resolution):
            point_list = [i]
            for appliance in appliances:
               point_list = [i, appliance.name, wattages[appliance.name]['avg'] + random.uniform(-wattages[appliance.name]['stdev'],wattages[appliance.name]['stdev'])]
               count += 1
               data_dict['points'].append(point_list)
         data.append(data_dict)
         db = influxdb.InfluxDBClient("localhost", 8086, "root", "root", "seads")
         if (db.write_points(data)):
            success = "Added {0} points successfully".format(count)
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

def influxdel(request):
   success = ""
   if request.method == 'POST':
      form = DatadelForm(request.POST)
      count = 0
      if form.is_valid():
         device = form.cleaned_data['device']
         db = influxdb.InfluxDBClient("localhost", 8086, "root", "root", "seads")
         series = db.query('list series')[0]['points']
         rg = re.compile('device.'+str(device.serial))
         for s in series:
            if rg.search(s[1]):
               db.query('drop series '+s[1])
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
