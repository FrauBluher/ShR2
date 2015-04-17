from rest_framework import serializers
from farmer.models import DeviceSettings


class DeviceSettingsSerializer(serializers.ModelSerializer):

   class Meta:
      model = DeviceSettings
      fields = ('device', 'main_channel', 'device_serial')