from rest_framework import serializers
from farmer.models import DeviceSettings


class DeviceSettingsSerializer(serializers.ModelSerializer):

   class Meta:
      model = DeviceSettings
      fields = ('device', 'device_serial', 'main_channel', 'transmission_rate_milliseconds','date_now')