from django.shortcuts import render, get_object_or_404

# Create your views here.


from django.views.decorators.csrf import csrf_exempt
from django.core.validators import RegexValidator
from django.http import HttpResponse
from django import forms
from django.core import serializers
from django.conf import settings

from rest_framework import permissions, authentication
from rest_framework.response import Response
from rest_framework import viewsets

from microdata.serializers import DeviceSerializer, EventSerializer, ApplianceSerializer, CircuitSerializer
from microdata.models import Device, Event, Appliance, CircuitType

from influxdb.influxdb08 import client as influxdb

from geoposition import Geoposition
import json
import time
import ast
import boto3

"""
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
"""

class KeyForm(forms.Form):
   """
   Form class used to generate a simple key form. Currently used when a user intends to add a new device via the webapp interface.
   """
   serial = forms.IntegerField()

class CircuitViewSet(viewsets.ModelViewSet):
   """
   API endpoint that allows circuits to be viewed or edited.
   """
   queryset = CircuitType.objects.all()
   serializer_class = CircuitSerializer

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
      if serial:
         device = Device.objects.create(ip_address=ip_address, serial=serial)
         data = serializers.serialize('json', [device,], fields=('owner', 'ip_address', 'secret_key', 'serial', 'name', 'registered','fanout_query_registered'))
         return HttpResponse(data, content_type="application/json", status=201)
      return HttpResponse("No serial provided", status=400)


class EventViewSet(viewsets.ModelViewSet):
   """
   API endpoint that allows events to be viewed or edited.
   """
   queryset = Event.objects.all()
   serializer_class = EventSerializer
   
   def create(self, request):
      """
      Custom create method.

      Used to parse the packets sent via the REST API. Since the packets are in a JSON array,
      the Django REST Framework has no native way of handling these, so we do it ourselves.
      """
      #try:
      # request is formed correctly
      if len(request.DATA.keys()) > 1:
         query = request.DATA
      # request is formed specifc to the ESP (bad)
      else:
         query = json.loads(request.DATA.keys()[0])
      serial = query.get('device').split('/')[-2:-1][0]
      device = get_object_or_404(Device, serial=serial)
      start = int(query.get('time')[0], 16)
      frequency = int(query.get('time')[1], 16)
      event = Event.objects.create(device=device, start=start, frequency=frequency, dataPoints=json.dumps(query.get('dataPoints')))
      device.ip_address = request.META.get('REMOTE_ADDR')
      device.save()
      return HttpResponse(content_type="application/json", status=201)
      #except:
      #   return HttpResponse("Bad Request: {0} {1}\n".format(type(query),request.DATA), status=400)
    
def new_device(request):
   """
      Function used to service a user's request to add a new :class:`microdata.models.Device`.

      **Context**

         ``form``
         :class:`microdata.views.KeyForm` object if user requests the form

         ``error``
         string - error description if present

         ``created``
         true/false if device is created

         ``Device``
         serialized :class:`microdata.models.Device` object if :class:`microdata.models.Device` is created


      **Templates:**

      `base/new_device/key.html`

      `base/new_device/help.html`

      `base/new_device/first.html`
      
      `base/new_device/result.html`


   """

   error = False
   created = False
   device = None
   template = 'base/new_device/first.html'
   form = None
   forward = request.GET.get('forward', None)
   page = request.GET.get('page', False)
   if page:
      if forward != None:
         if page == 'first':
            if forward == 'true':
               form = KeyForm()
               template = 'base/new_device/key.html'
            else:
               template = 'base/new_device/help.html'
         elif page == 'help':
            if forward == 'true':
               template = 'base/new_device/first.html'
         elif page == 'key':
            if forward == 'false':
               template = 'base/new_device/first.html'
      else:
         template = 'base/new_device/first.html'
   else:
      if request.method == 'POST':
         template = 'base/new_device/result.html'
         form = KeyForm(request.POST)
         if form.is_valid():
            """
            secret_key =  str(form.cleaned_data['digit1'])
            secret_key += str(form.cleaned_data['digit2'])
            secret_key += str(form.cleaned_data['digit3'])
            
            secret_key += form.cleaned_data['char1']
            secret_key += form.cleaned_data['char2']
            secret_key += form.cleaned_data['char3']
            secret_key += form.cleaned_data['char4']
            """
            serial = str(form.cleaned_data['serial'])
            devices = Device.objects.filter(registered=False, serial=serial)
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
   return render(request, template, {
             'form':form,
             'error':error,
             'created':created,
             'device':device
             })

def timestamp(request):
   """
   Function to return the server's time in milliseconds.

   This function is possibly deprecated. Devices should now get the server time
   from :class:`farmer.DeviceSettingsViewSet`.
   """
   milliseconds = time.time()*1000
   return HttpResponse(json.dumps(milliseconds), content_type="application/json")

def initiate_job_to_glacier(request, requester, end_time):
   """
   Experimental class to demonstrate the possibility to archive old data to Amazon Glacier.

   Requires an Amazon AWS key to be set in the environment variables.
   """
   glacier = boto3.client('glacier', region_name='us-west-2')
   with open(settings.STATIC_PATH+'archive_ids.log', 'r') as f:
      archives = f.read()
      for archive in archives.split(';'):
         try:
            archive = json.loads(archive)
            # download up to the time specified
            if archive['timeEnd'] < end_time:
               parameters = {
                  'Type': 'archive-retrieval',
                  'ArchiveId': archive['archiveId'],
                  'Description': str(requester.pk)
               }
               job = glacier.initiate_job(vaultName=settings.GLACIER_VAULT_NAME, jobParameters=parameters)
               return job['HTTPStatusCode']
         except:
            return 400
   return 404






