from django.shortcuts import render
from farmer.models import DeviceSettings
from farmer.serializers import DeviceSettingsSerializer
from rest_framework.response import Response
from django.shortcuts import get_object_or_404

import time

from rest_framework import viewsets

# Create your views here.

class DeviceSettingsViewSet(viewsets.ModelViewSet):
   """
   API endpoint that allows devicesettings to be viewed or edited.
   """
   queryset = DeviceSettings.objects.all()
   serializer_class = DeviceSettingsSerializer
   
   def retrieve(self, request, pk=None):
      queryset = DeviceSettings.objects.all()
      setting = get_object_or_404(queryset, pk=pk)
      setting.date_now = int(time.time()*1000)
      setting.save()
      serializer = DeviceSettingsSerializer(setting)
      return Response(serializer.data)
   
   def list(self, request):
      queryset = DeviceSettings.objects.all()
      for setting in queryset:
         setting.date_now = int(time.time()*1000)
         setting.save()
      serializer = DeviceSettingsSerializer(queryset, many=True)
      return Response(serializer.data)