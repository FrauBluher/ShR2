from microdata.models import Device, Event, Appliance
from rest_framework import serializers
from home.serializers import UserSerializer

class ApplianceSerializer(serializers.HyperlinkedModelSerializer):
	class Meta:
		model = Appliance
		fields = ('name', 'serial')

class DeviceSerializer(serializers.HyperlinkedModelSerializer):
   
   class Meta:
      model = Device
      fields = ('owner', 'secret_key', 'serial', 'name', 'zipcode', 'private', 'registered','fanout_query_registered')
      

class EventSerializer(serializers.HyperlinkedModelSerializer):
   
   class Meta:
      model = Event
      fields = ('device', 'event_code', 'appliance', 'timestamp', 'wattage')
