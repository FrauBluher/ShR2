# Create your views here.
from django.http import HttpResponse, HttpResponseRedirect
from django.views.decorators.csrf import csrf_exempt
from microdata.models import Event, Device, Appliance
from django import forms
from django.shortcuts import render
from django.forms import ModelChoiceField

import git
import random

class DeviceModelChoiceField(ModelChoiceField):
   def label_from_instance(self, obj):
      return "%s (%i)" % (obj, obj.serial)

class DatagenForm(forms.Form):
   device = DeviceModelChoiceField(label='Device', queryset=Device.objects.all())
   appliances = forms.ModelMultipleChoiceField(label='Appliances', queryset=Appliance.objects.all())
   start = forms.IntegerField(label='Start')
   stop = forms.IntegerField(label='Stop') 

class DatadelForm(forms.Form):
   device = DeviceModelChoiceField(label='Device', queryset=Device.objects.all())

class DevForm(forms.Form):
   method = forms.ChoiceField(choices=(('datagen','datagen'),('datadel','datadel')))

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
   
def datagen(request):
   success = ""
   if request.method == 'POST':
      form = DatagenForm(request.POST)
      if form.is_valid():
         device = form.cleaned_data['device']
         appliances = form.cleaned_data['appliances']
         start = form.cleaned_data['start']
         stop = form.cleaned_data['stop']
         events = []
         averages = {'Computer':100, 'Toaster':20, 'Refrigerator':400, 'Television':60}
         for i in range(start, stop):
             for appliance in appliances:
                 wattage = averages[appliance.name] + random.uniform(-10,10)
                 event = Event(device=device, timestamp=i*1000, wattage=wattage, appliance=appliance)
                 event.save()
                 events.append("{0}: {1}: {2} - {3}".format(event.device, event.timestamp, event.wattage, event.appliance))
         success = "Added %i events successfully" % (stop - start)
   else:
      form = DatagenForm()
   title = "Debug - Data Generation"
   description = "Use this form to submit random generated data with 1s resolution for the device chosen."
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
         success = "Deleted %i events successfully" % count
   else:
      form = DatadelForm()
   title = "Debug - Data Deletion"
   description = "Use this form to delete data for the device chosen."
   return render(request, 'debug.html', {'title':title,'description':description,'form':form, 'success':success})

def dev(request):
   methods = {'datagen':datagen, 'datadel':datadel}
   if request.method == 'POST':
      form = DevForm(request.POST)
      if form.is_valid():
         method = form.cleaned_data['method']
         if method in methods:
            return HttpResponse(methods[method](request))
         else: return HttpResponse(status=404)
   else:
      form = DevForm()
   title = "Development - Method Select"
   description = "Select a debug method"
   return render(request, 'debug.html', {'title':title,'description':description,'form':form})
