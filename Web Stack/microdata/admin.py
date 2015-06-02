from django.contrib import admin

from microdata.models import Device, Appliance, Event, CircuitType, Circuit
from webapp.models import DeviceWebSettings
from farmer.admin import DeviceSettingsInline
# Register your models here.

class DeviceWebSettingsInline(admin.StackedInline):
   model = DeviceWebSettings
   can_delete = False
   verbose_name_plural = 'devicesettings'

class CircuitAdmin(admin.ModelAdmin):
   list_display = ('name','circuittype','pk',)

class CircuitTypeAdmin(admin.ModelAdmin):
   list_display = ('name','pk',)

class DeviceAdmin(admin.ModelAdmin):
   """
   This class gives an administrator direct access to the device model.

   Most of the fields of the device model can be modified through the custom settings interface
   on the web application, but this interface gives the administrator direct control.
   """
   list_display = ('name','owner','serial','position','secret_key','registered','fanout_query_registered',)
   search_fields = ('name','serial')
   readonly_fields=('secret_key',)
   inlines = (DeviceWebSettingsInline,DeviceSettingsInline,)

class ApplianceAdmin(admin.ModelAdmin):
   """
   Class that allows administrator access to the Appliance models.

   This class was included to give the administartor the ability to add new appliances
   as the algorithms detect them. This is exposed so that an administrator can extend
   the functionality of the Appliances without having to touch the source code.
   """
   list_display = ('name','pk','serial','chart_color',)


#admin.site.unregister(Device)
admin.site.register(Device, DeviceAdmin)
admin.site.register(Appliance, ApplianceAdmin)
admin.site.register(Event)
admin.site.register(CircuitType, CircuitTypeAdmin)
admin.site.register(Circuit, CircuitAdmin)