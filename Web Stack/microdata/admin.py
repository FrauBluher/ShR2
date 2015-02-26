from django.contrib import admin

from microdata.models import Device
# Register your models here.

class DeviceAdmin(admin.ModelAdmin):
   list_display = ('name','owner','serial','position','secret_key','registered','fanout_query_registered',)
   search_fields = ('name','serial')
   readonly_fields=('secret_key',)


admin.site.register(Device, DeviceAdmin)
