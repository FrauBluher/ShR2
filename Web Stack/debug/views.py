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
         return HttpResponse(events)
   else:
      form = DatagenForm()
   title = "Debug - Data Generation"
   description = "Use this form to submit random generated data for the device chosen."
   return render(request, 'debug.html', {'title':title,'description',description,'form':form})

def datadel(request):
  if request.method == 'POST':
    form = DatadelForm(request.POST)
    if form.is_valid():
       device = form.cleaned_data['device']
       events = Event.objects.filter(device=device)
       events.delete()
  else:
    form = DatadelForm()
  title = "Debug - Data Deletion"
  description = "Use this form to delete data for the device chosen."
  return render(request, 'debug.html', {'title':title,'description',description,'form':form})
      
