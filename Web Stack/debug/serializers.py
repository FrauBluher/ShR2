from debug.models import TestEvent
from rest_framework import serializers
from microdata.models import Device





class TestEventSerializer(serializers.HyperlinkedModelSerializer):
    class Meta:
        model = TestEvent
        fields = (
            'device', 
            'dataPoints',  
        )
