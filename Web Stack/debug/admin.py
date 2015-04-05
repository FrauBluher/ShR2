from django.contrib import admin

from debug.models import TestEvent
# Register your models here.


#admin.site.unregister(Device)
admin.site.register(TestEvent)