from microdata.models import Device, Event
from rest_framework import serializers
from home.serializers import UserSerializer


class DeviceSerializer(serializers.HyperlinkedModelSerializer):
   #owner = UserSerializer()
   class Meta:
      model = Device
      fields = ('owner', 'serial', 'name', 'zipcode', 'private')

class EventSerializer(serializers.HyperlinkedModelSerializer):
   #device = DeviceSerializer()
   class Meta:
      model = Event
      fields = ('device', 'event_code', 'wattage', 'timestamp')