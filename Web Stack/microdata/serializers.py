from microdata.models import Device, Event, Appliance, CircuitType
from rest_framework import serializers
from home.serializers import UserSerializer

class CircuitSerializer(serializers.HyperlinkedModelSerializer):
	class Meta:
		model = CircuitType
		fields = ('name','appliances','chart_color')

class ApplianceSerializer(serializers.HyperlinkedModelSerializer):
	class Meta:
		model = Appliance
		fields = ('name', 'serial', 'chart_color')

class DeviceSerializer(serializers.HyperlinkedModelSerializer):
   
   class Meta:
      model = Device
      fields = ('owner', 'ip_address', 'secret_key', 'serial', 'name', 'registered','fanout_query_registered','channel_1','channel_2','channel_3','data_retention_policy','kilowatt_hours_monthly','kilowatt_hours_daily')
      

class EventSerializer(serializers.HyperlinkedModelSerializer):
   
   class Meta:
      model = Event
      fields = ('device', 'dataPoints')
