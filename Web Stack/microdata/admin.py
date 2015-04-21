from django.contrib import admin

from microdata.models import Device, Appliance, Event, CircuitType, Circuit
from webapp.models import DeviceWebSettings
from farmer.admin import DeviceSettingsInline
# Register your models here.

class DeviceWebSettingsInline(admin.StackedInline):
   model = DeviceWebSettings
   can_delete = False
   verbose_name_plural = 'devicesettings'

class DeviceAdmin(admin.ModelAdmin):
   list_display = ('name','owner','serial','position','secret_key','registered','fanout_query_registered',)
   search_fields = ('name','serial')
   readonly_fields=('secret_key',)
   inlines = (DeviceWebSettingsInline,DeviceSettingsInline,)

class ApplianceAdmin(admin.ModelAdmin):
   list_display = ('name','pk','serial','chart_color',)


#admin.site.unregister(Device)
admin.site.register(Device, DeviceAdmin)
admin.site.register(Appliance, ApplianceAdmin)
admin.site.register(Event)
admin.site.register(CircuitType)
admin.site.register(Circuit)