from django.shortcuts import render

# Create your views here.

from microdata.models import Device, Event, Appliance
from rest_framework import viewsets
from microdata.serializers import DeviceSerializer, EventSerializer, ApplianceSerializer
from rest_framework import permissions, authentication
from rest_framework.response import Response
from django import forms
from django.core.validators import RegexValidator
import random
import string

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
   #permission_classes = (permissions.IsAuthenticated,)
   #authentication_classes = (authentication.TokenAuthentication,)

class DeviceViewSet(viewsets.ModelViewSet):
   """
   API endpoint that allows devices to be viewed or edited.
   """
   queryset = Device.objects.all()
   serializer_class = DeviceSerializer
   #lookup_field = 'serial'
   #permission_classes = (permissions.IsAuthenticated,)
   #authentication_classes = (authentication.TokenAuthentication,)
   def restore_object(self, attrs, instance=None):
        if instance is not None:
            instance.serial = attrs.get('serial', instance.serial)
            instance.name = attrs.get('name', instance.url)
            instance.zipcode = attrs.get('zipcode', instance.zipcode)
            instance.private = attrs.get('private', instance.private)
            instance.registered = attrs.get('registered', instance.registered)
            return instance

        secret_key =  ''.join(random.choice(string.digits) for i in range(3))
        secret_key += ''.join(random.choice(string.ascii_uppercase) for i in range(4))
        del attrs['secret_key']

        device = Device(**attrs)
        device.secret_key = secret_key

        return device

class EventViewSet(viewsets.ModelViewSet):
   """
   API endpoint that allows events to be viewed or edited.
   """
   queryset = Event.objects.all()
   serializer_class = EventSerializer
   #permission_classes = (permissions.IsAuthenticated,)
   #authentication_classes = (authentication.TokenAuthentication,)
    
def new_device_key(request):
   error = False
   created = False
   if request.user.is_authenticated():
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
            devices = Device.objects.filter(registered=False, secret_key = secret_key)
            device = devices[0] if devices else None
            if device:
               device.owner = request.user
               device.registered = True
               device.save()
               created = True
            else: error = True
      else:
         form = KeyForm()
      return render(request, 'base/key.html', {
             'form':form,
             'error':error,
             'created':created
             })
    