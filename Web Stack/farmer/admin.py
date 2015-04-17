from django.contrib import admin
from farmer.models import DeviceSettings

# Register your models here.


class DeviceSettingsInline(admin.StackedInline):
   model = DeviceSettings
   can_delete = False
   
class DeviceSettingsAdmin(admin.ModelAdmin):
   verbose_name_plural = 'devicesettings'
   
admin.site.register(DeviceSettings, DeviceSettingsAdmin)