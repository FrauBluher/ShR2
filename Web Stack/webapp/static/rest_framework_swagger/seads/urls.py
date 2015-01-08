from django.conf.urls import url, include
from rest_framework import routers, serializers, viewsets
from microdata import views as microdata_views
from home import views as home_views
from django.contrib import admin

router = routers.DefaultRouter()
router.register(r'device-api', microdata_views.DeviceViewSet)
router.register(r'event-api', microdata_views.EventViewSet)
router.register(r'users', home_views.UserViewSet)

# Wire up our API using automatic URL routing.
urlpatterns = [
    url(r'^admin/', include(admin.site.urls)),
    url(r'^$', 'home.views.index'), 
    url(r'^api/', include(router.urls)),
    url(r'^docs/', include('rest_framework_swagger.urls')),
    url(r'^signin/$', 'home.views.signin'),
    url(r'^signout/$', 'home.views.signout'),
    url(r'^register/$', 'home.views.register'),
    url(r'^account/$', 'home.views.account'),
    url(r'^data/$', 'webapp.views.landing'),
    url(r'^devices/(?P<serial>\d+)/$', 'webapp.views.devices'),
    url(r'^dashboard/$', 'webapp.views.dashboard'),
    url(r'^charts/device/(?P<serial>\d+)/$', 'webapp.views.charts')
]
