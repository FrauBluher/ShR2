from django.shortcuts import render
from farmer.models import DeviceSettings
from farmer.serializers import DeviceSettingsSerializer

from rest_framework import viewsets

# Create your views here.

class DeviceSettingsViewSet(viewsets.ModelViewSet):
   """
   API endpoint that allows devicesettings to be viewed or edited.
   """
   queryset = DeviceSettings.objects.all()
   serializer_class = DeviceSettingsSerializer