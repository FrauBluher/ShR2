from django.shortcuts import render

# Create your views here.

from microdata.models import Device, Event, Appliance
from rest_framework import viewsets
from microdata.serializers import DeviceSerializer, EventSerializer, ApplianceSerializer
from rest_framework import permissions, authentication
from rest_framework.response import Response

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

class EventViewSet(viewsets.ModelViewSet):
    """
    API endpoint that allows events to be viewed or edited.
    """
    queryset = Event.objects.all()
    serializer_class = EventSerializer
    #permission_classes = (permissions.IsAuthenticated,)
    #authentication_classes = (authentication.TokenAuthentication,)
    