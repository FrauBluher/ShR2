from django.contrib import admin

from microdata.models import Device
from webapp.models import DeviceSettings
# Register your models here.

class DeviceSettingsInline(admin.StackedInline):
	model = DeviceSettings
	can_delete = False
	verbose_name_plural = 'devicesettings'

class DeviceAdmin(admin.ModelAdmin):
   list_display = ('name','owner','serial','position','secret_key','registered','fanout_query_registered',)
   search_fields = ('name','serial')
   readonly_fields=('secret_key',)
   inlines = (DeviceSettingsInline,)

#admin.site.unregister(Device)
admin.site.register(Device, DeviceAdmin)