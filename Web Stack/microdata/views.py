from django.shortcuts import render

# Create your views here.


from django.views.decorators.csrf import csrf_exempt
from django.core.validators import RegexValidator
from django.http import HttpResponse
from django import forms
from django.core import serializers

from rest_framework import permissions, authentication
from rest_framework.response import Response
from rest_framework import viewsets

from microdata.serializers import DeviceSerializer, EventSerializer, ApplianceSerializer
from microdata.models import Device, Event, Appliance

from influxdb.influxdb08 import client as influxdb

from geoposition import Geoposition
import json
import time
import ast


class KeyForm(forms.Form):
   alphabetic = RegexValidator(r'^[a-zA-Z]*$', 'Only alphabetical characters are allowed.')
   attrs_digit = {'size':1, 'maxlength':1, 'style':'text-align: center; margin-right: 0.5rem;', 'class':'keyField single-cell digit'}
   attrs_digit_last = {'size':1, 'maxlength':1, 'style':'text-align: center; margin-right: 1.5rem;', 'class':'keyField single-cell digit last'}
   attrs_char = {'size':1, 'maxlength':1, 'style':'text-align: center; margin-right: 0.5rem;', 'class':'keyField single-cell char'}
   attrs_char_last = {'size':1, 'maxlength':1, 'style':'text-align: center;', 'class':'keyField single-cell char last'}

   digit1 = forms.IntegerField(widget=forms.TextInput(attrs=attrs_digit), max_value=9)
   digit2 = forms.IntegerField(widget=forms.TextInput(attrs=attrs_digit), max_value=9)
   digit3 = forms.IntegerField(widget=forms.TextInput(attrs=attrs_digit_last), max_value=9)
   
   char1 = forms.CharField(widget=forms.TextInput(attrs=attrs_char), max_length=1, validators=[alphabetic])
   char2 = forms.CharField(widget=forms.TextInput(attrs=attrs_char), max_length=1, validators=[alphabetic])
   char3 = forms.CharField(widget=forms.TextInput(attrs=attrs_char), max_length=1, validators=[alphabetic])
   char4 = forms.CharField(widget=forms.TextInput(attrs=attrs_char_last), max_length=1, validators=[alphabetic])
   

class ApplianceViewSet(viewsets.ModelViewSet):
   """
   API endpoint that allows appliances to be viewed or edited.
   """
   queryset = Appliance.objects.all()
   serializer_class = ApplianceSerializer


class DeviceViewSet(viewsets.ModelViewSet):
   """
   API endpoint that allows devices to be viewed or edited.
   """
   queryset = Device.objects.all()
   serializer_class = DeviceSerializer
   
   def create(self, request):
      ip_address = request.META.get('REMOTE_ADDR')
      serial = request.DATA.get('serial')
      device = Device.objects.create(ip_address=ip_address, serial=serial)
      data = serializers.serialize('json', [device,], fields=('owner', 'ip_address', 'secret_key', 'serial', 'name', 'registered','fanout_query_registered'))
      return HttpResponse(data, content_type="application/json", status=201)

class EventViewSet(viewsets.ModelViewSet):
   """
   API endpoint that allows events to be viewed or edited.
   """
   queryset = Event.objects.all()
   serializer_class = EventSerializer
   
   def create(self, request):
      query = request.DATA.keys()[0]
      query = json.loads(query)
      #try:
      serial = query.get('device').split('/')[-2:-1][0]
      device = Device.objects.get(serial=serial)
      event = Event.objects.create(device=device, dataPoints = json.dumps(query.get('dataPoints')))
      device.ip_address = request.META.get('REMOTE_ADDR')
      device.save()
      data = serializers.serialize('json', [event,], fields=('device', 'dataPoints'))
      return HttpResponse(query, content_type="application/json", status=201)
      #except:
      #   return HttpResponse("Bad Request: {0} {1}\n".format(type(query),query), status=400)
    
def new_device_key(request):
   error = False
   created = False
   device = None
   if request.method == 'POST':
      form = KeyForm(request.POST)
      if form.is_valid():
         secret_key =  str(form.cleaned_data['digit1'])
         secret_key += str(form.cleaned_data['digit2'])
         secret_key += str(form.cleaned_data['digit3'])
         
         secret_key += form.cleaned_data['char1']
         secret_key += form.cleaned_data['char2']
         secret_key += form.cleaned_data['char3']
         secret_key += form.cleaned_data['char4']
         devices = Device.objects.filter(registered=False, secret_key=secret_key.upper())
         device = devices[0] if devices else None
         if device:
            device.owner = request.user
            device.registered = True
            device.save()
            created = True
            form = False
         else: error = True
   else:
      form = KeyForm()
   return render(request, 'base/key.html', {
             'form':form,
             'error':error,
             'created':created,
             'device':device
             })

def timestamp(request):
   milliseconds = time.time()*1000
   return HttpResponse(json.dumps(milliseconds), content_type="application/json")

    
