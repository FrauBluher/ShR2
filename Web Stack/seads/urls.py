from django.conf.urls import url, include
from rest_framework import routers, serializers, viewsets
from microdata.views import ApplianceViewSet, DeviceViewSet, EventViewSet
from farmer.views import DeviceSettingsViewSet
from home.views import UserViewSet
from django.contrib import admin
from debug.views import TestEventViewSet

router = routers.DefaultRouter()
router.register(r'appliance-api', ApplianceViewSet)
router.register(r'device-api', DeviceViewSet)
router.register(r'event-api', EventViewSet)
router.register(r'settings-api', DeviceSettingsViewSet)
router.register(r'users', UserViewSet)

router.register(r'testevent', TestEventViewSet)

admin.autodiscover()
# Wire up our API using automatic URL routing.
urlpatterns = [
    url(r'^admin/', include(admin.site.urls)),
    url(r'^$', 'home.views.index'), 
    url(r'^signin/$', 'home.views.signin'),
    url(r'^signout/$', 'home.views.signout'),
    url(r'^register/$', 'home.views.register'),
    url(r'^account/$', 'home.views.account'),
    url(r'^settings/$', 'webapp.views.settings'),
    url(r'^settings/account/$', 'webapp.views.settings_account'),
    url(r'^settings/device/(?P<serial>\d+)$', 'webapp.views.settings_device'),
    url(r'^settings/change_device/$', 'webapp.views.settings_change_device'),
    url(r'^settings/dashboard/$', 'webapp.views.settings_dashboard'),
    url(r'^settings/device/status/$', 'webapp.views.device_status'),
    url(r'^settings/device/location/(?P<serial>\d+)$', 'webapp.views.device_location'),
    url(r'^settings/device/remove/(?P<serial>\d+)$', 'webapp.views.remove_device'),
    url(r'^settings/device/export/$', 'webapp.views.export_data'),
    url(r'^data/$', 'webapp.views.landing'),
    url(r'^dashboard/$', 'webapp.views.dashboard'),
    url(r'^dashboard/update/$', 'webapp.views.dashboard_update'),
    url(r'^charts/device/(?P<serial>\d+)/data/$', 'webapp.views.device_data'),
    url(r'^charts/device/(?P<serial>\d+)/chart/$', 'webapp.views.device_chart'),
    #url(r'^charts/heatmap/$', 'webapp.views.heatmap'),
    url(r'^charts/default_chart/$', 'webapp.views.default_chart'),
    url(r'^charts/billing_information/$', 'webapp.views.billing_information'),
    url(r'^charts/circuits_information/$', 'webapp.views.circuits_information'),
    url(r'^new_device/', 'microdata.views.new_device'),
    url(r'^api/timestamp/$', 'microdata.views.timestamp'),
    url(r'^device/get_wattage_usage/', 'webapp.views.get_wattage_usage'),
    
    # Development URLs
    url(r'^api/', include(router.urls)),
    url(r'^docs/', include('rest_framework_swagger.urls')),
    url(r'^gitupdate/$', 'debug.views.gitupdate'),
    url(r'^echo/', 'debug.views.echo'),
    url(r'^echo/(?P<args>\w+)$', 'debug.views.echo_args'),
    url(r'^datagen/$', 'debug.views.datagen'),
    url(r'^datadel/$', 'debug.views.datadel'),
    url(r'^influxgen/$', 'debug.views.influxgen'),
    url(r'^influxdel/$', 'debug.views.influxdel'),
]
