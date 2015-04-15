from django.shortcuts import render
from django.contrib.auth.decorators import login_required
from microdata.models import Device, Event, Appliance
from django.views.decorators.csrf import csrf_exempt
from django.contrib.auth.models import User
from django import forms
from django.http import HttpResponse
from django.conf import settings as django_settings
from sets import Set
from collections import defaultdict
from django.contrib.auth import authenticate, login
from webapp.models import IntervalNotification, UtilityCompany, RatePlan, Territory, DashboardSettings
from geoposition import Geoposition

from django.db import IntegrityError

from datetime import datetime
import json
import time
import re
from influxdb import client as influxdb

# Create your views here.

def make_choices(queryset):
  choices = []
  for choice in queryset:
    choices.append((choice.pk, choice.description))
  return choices

# UserForm is a customized version of an AuthenticationForm
class SettingsForm(forms.Form):
  # User settings
  new_username = forms.CharField(max_length=254,
                                 min_length=1,
                                 required=False,
                                 widget=forms.TextInput(attrs={
                                    'class' : 'form-control input-md'}))
  error_messages = {
    'password_mismatch': ("The two password fields didn't match."),
  }
  password1 = forms.CharField(label=("Password"),
      widget=forms.PasswordInput(attrs={'class' : 'form-control input-md'}),
      required=False)
  password2 = forms.CharField(label=("Password confirmation"),
      widget=forms.PasswordInput(attrs={'class' : 'form-control input-md'}),
      help_text=("Enter the same password as above, for verification."),
      required=False)

  notification_choices = []
  notifications = forms.ChoiceField(
      widget=forms.CheckboxSelectMultiple(),
      # Default choices
      choices=(
         notification_choices,
        ),
      required=False
  )

  # Device settings
  new_name = forms.CharField(max_length=254,
                             min_length=1,
                             required=False,
                             widget=forms.TextInput(attrs={
                              'class' : 'form-control input-md'
                              })
                            )
  utility_company_choices = []
  rate_plan_choices = []
  territory_choices = []

  utility_companies = forms.ChoiceField(
    widget=forms.Select(attrs={
          'class':'form-control',
          'id':'company-dropdown'
          }
        ),
    choices=(
      utility_company_choices
    ),
    required=False
  )
  rate_plans = forms.ChoiceField(
    widget=forms.Select(attrs={
          'class':'form-control',
          'id':'rate-dropdown'
          }
        ),
    choices=(
      rate_plan_choices
    ),
    required=False
  )
  territories = forms.ChoiceField(
    widget=forms.Select(attrs={
          'class':'form-control',
          'id':'territory-dropdown'
          }
        ),
    choices=(
      territory_choices
    ),
    required=False
  )
  stack = forms.BooleanField(
    required=False,
  )

  def __init__(self, *args, **kwargs):
    user = kwargs.pop('user', None)
    device = kwargs.pop('device', None)
    super(SettingsForm, self).__init__(*args, **kwargs)
    if user:
      self.notification_choices = make_choices(IntervalNotification.objects.all())
      self.fields['notifications'] = forms.ChoiceField(
        widget=forms.CheckboxSelectMultiple(),
        choices=(
           self.notification_choices
          ),
        required=False
      )
      #self.fields['stack'] = forms.BooleanField(
      #  default = DashboardSettings.objects.get(user=user),
      #)
    if device:
      self.utility_company_choices = make_choices(UtilityCompany.objects.all())
      self.rate_plan_choices = make_choices(RatePlan.objects.all())
      self.territory_choices = make_choices(Territory.objects.all())

      self.fields['utility_companies'] = forms.ChoiceField(
        widget=forms.Select(attrs={
          'class':'form-control',
          'id':'company-dropdown'
          }
        ),
        choices=(
          self.utility_company_choices
        ),
        required=False
      )
      self.fields['rate_plans'] = forms.ChoiceField(
        widget=forms.Select(attrs={
          'class':'form-control',
          'id':'rate-dropdown'
          }
        ),
        choices=(
          self.rate_plan_choices
        ),
        required=False
      )
      self.fields['territories'] = forms.ChoiceField(
        widget=forms.Select(attrs={
          'class':'form-control',
          'id':'territory-dropdown'
          }
        ),
        choices=(
          self.territory_choices
        ),
        required=False
      )

  def get_notifications(self, *args, **kwargs):
    user = kwargs.pop('user', None)
    if user:
      return make_choices(user.usersettings.interval_notification.all())

  def get_utility_companies(self, *args, **kwargs):
    device = kwargs.pop('device', None)
    if device:
      return make_choices(device.devicesettings.utility_companies.all())

  def get_rate_plans(self, *args, **kwargs):
    device = kwargs.pop('device', None)
    if device:
      return make_choices(device.devicesettings.rate_plans.all())

  def get_territories(self, *args, **kwargs):
    device = kwargs.pop('device', None)
    if device:
      return make_choices(device.devicesettings.territories.all())

  def clean_password2(self):
    password1 = self.cleaned_data.get("password1")
    password2 = self.cleaned_data.get("password2")
    if password1 and password2 and password1 != password2:
        raise forms.ValidationError(self.error_messages['password_mismatch'])
    return password2



def chartify(data):
   warnings.warn("chartify method no longer used for InfluxDB.", DeprecationWarning)
   values = []
   for time in data:
      tmp = [time]
      for value in data[time]:
         tmp.append(value)
      values.append(tmp)
   return values


@login_required(login_url='/signin/')
def landing(request):
   user = request.user.id
   my_devices = Device.objects.filter(owner=user)
   context = {'my_devices': my_devices}
   return render(request, 'base/landing.html', context)


@login_required(login_url='/signin/')
def dashboard(request):
   user = request.user.id
   if request.user.is_authenticated():
      my_devices = Device.objects.filter(owner=user)
   else: my_devices = None
   db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
   context = {'my_devices': my_devices,
              'server_time': time.time()*1000,
              }
   return render(request, 'base/dashboard.html', context)


def merge_subs(lst_of_lsts):
    res = []
    for row in lst_of_lsts:
        for i, resrow in enumerate(res):
            if row[0]==resrow[0]:
                res[i] += row[1:]
                break
        else:
            res.append(row)
    return res


def group_by_mean(serial, unit, start, stop, localtime):
   if unit == 'y': unit = 'm'
   if (start == ''): start = 'now() - 1d'
   else: start = '\''+datetime.fromtimestamp(int(float(start))).strftime('%Y-%m-%d %H:%M:%S')+'\''
   if (stop == ''): stop = 'now()'
   else: stop = '\''+datetime.fromtimestamp(int(float(stop))).strftime('%Y-%m-%d %H:%M:%S')+'\''
   db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
   result = db.query('list series')[0]
   appliances = Set()
   for series in result['points']:
      rg = re.compile('device.'+str(serial))
      if re.match(rg, series[1]):
         appliance = series[1].split('device.'+str(serial)+'.')
         if (len(appliance) < 2): continue
         else: appliances.add(appliance[-1])
   mean = {}
   to_merge = []
   for appliance in appliances:
      query = 'select * from 1'+unit+'.device.'+str(serial)+'.'+appliance+' where time > '+start+' and time < '+stop
      group = []
      group = db.query(query)
      if (len(group)): group = group[0]['points']
      #else: return None
      # hack. Remove sequence_number and timezone offset for GMT
      new_group = []
      for s in group:
         # use 28800 for daylight savings (-8 PDT)
         # use 25200 for normal (-7 PDT)
         #offset = time.time() - localtime
         s = [int(round(s[0]-25200)),s[2]]
         new_group.append(s)
      to_merge += new_group
   data = merge_subs(to_merge)
   for point in data:
      point.insert(1, sum(point[1:]))
   if (len(data) < 1): data.append([None,])
   data = {'data': data,
           'unit': unit,
           'dataLimitFrom': data[len(data)-1][0],
           'dataLimitTo': data[0][0],
          }
   return data

@login_required(login_url='/signin/')
def default_chart(request):
   if request.method == 'GET':
      user = User.objects.get(username=request.user)
      devices = Device.objects.filter(owner=user)
      db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
      context = {'my_devices': devices,
                 'server_time': time.time()*1000,
                }
      return render(request, 'base/dashboard.html', context)

      
@login_required(login_url='/signin/')
def get_wattage_usage(request):
   context = {}
   if request.method == 'GET':
      user = User.objects.get(username=request.user)
      serial = request.GET.get('serial')
      device = Device.objects.get(serial=serial)
      if device.owner == user:
        db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
        result = db.query('list series')[0]
        appliances = Set()
        for series in result['points']:
           rg = re.compile('device.'+str(device.serial))
           if re.match(rg, series[1]):
              appliance = series[1].split('device.'+str(device.serial)+'.')
              if (len(appliance) < 2): continue
              else:
                 appliances.add(appliance[-1])
        average_wattage = 0
        current_wattage = 0
        for appliance in appliances:
           try:
              average_wattage += db.query('select mean(wattage) from device.'+str(device.serial)+'.'+appliance)[0]['points'][0][1]
              this_wattage = db.query('select * from 1m.device.'+str(device.serial)+'.'+appliance)[0]['points'][0]
              if this_wattage[0] > time.time() - 1000:
                 current_wattage += this_wattage[2]
           except:
              pass
        context['average_wattage'] = int(average_wattage)
        context['current_wattage'] = int(current_wattage)
   return HttpResponse(json.dumps(context), content_type="application/json")

   
@login_required(login_url='/signin/')
def device_data(request, serial):
   context = None
   if request.method == 'GET':
      user = User.objects.get(username=request.user)
      device = Device.objects.get(serial=serial)
      localtime = int(float(request.GET.get('localtime', time.time())))
      if device.owner == user:
         unit = request.GET.get('unit','')
         start = request.GET.get('from','')
         stop = request.GET.get('to','')
         context = json.dumps(group_by_mean(serial,unit,start,stop, localtime))
   return HttpResponse(context, content_type="application/json")


@login_required(login_url='/signin/')
def device_chart(request, serial):
   context = {}
   if request.method == 'GET':
      stack = request.GET.get('stack')
      user = User.objects.get(username=request.user)
      device = Device.objects.get(serial=serial)

      if device.owner == user:
         context['device'] = device
         db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
         result = db.query('list series')[0]
         appliance_names = Set()

         for series in result['points']:
            rg = re.compile('device.'+str(device.serial))
            if re.match(rg, series[1]):
               appliance_name = series[1].split('device.'+str(device.serial)+'.')
               if (len(appliance_name) < 2): continue
               else: appliance_names.add(appliance_name[-1])

         context['appliances'] = []
         # place Unknown appliance at front of list
         unknown_appliance = Appliance.objects.get(serial=0)
         if unknown_appliance.name in appliance_names:
            context['appliances'].append(unknown_appliance)
         for name in appliance_names:
            appliance = Appliance.objects.get(name=name)
            if appliance != unknown_appliance:
               context['appliances'].append(Appliance.objects.get(name=name));

         context['server_time'] = time.time()*1000
         context['stack'] = stack == 'true'
   return render(request, 'base/chart.html', context)
   

@login_required(login_url='/signin/')
def charts_deprecated(request, serial, unit):
   warnings.warn("Generating chart data from sqlite deprecated. See new charts() using InfluxDB.", DeprecationWarning)
   if request.method == 'GET':
      user = request.user.id
      devices = Device.objects.filter(owner=user, serial=serial)
      device = devices[0] if devices else None
      events = Event.objects.filter(device=device)
      response_data = {}
      response_data['unit'] = unit
      response_data['values'] = []
      if events:
         values = defaultdict(list)
         appliances = Set()
         [appliances, values] = units[unit](events, appliances, values)
         response_data['weather'] = get_weather(str(device.zipcode))
         response_data['appliances'] = list(appliances)
         response_data['values'] = chartify(values)
         response_data['dataLimitFrom'] = events[0].timestamp
         response_data['dataLimitTo'] = events[len(events)-1].timestamp
      return HttpResponse(json.dumps(response_data), content_type="application/json")

def device_is_online(device):
  if device:
    db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
    result = [0,]
    try:
      result = db.query('select * from device.'+str(device.serial)+' limit 1;')[0]['points'][0]
    except:
      # bypasses 400 error thrown by InfluxDB if series does not exist
      pass
    return int(time.time()) - int(result[0]) < 4000

class Object:
   def __init__(self, serial):
      self.serial = serial
    
@login_required(login_url='/signin/')
def device_status(request):
   connected = False
   if request.method == 'GET':
      serial = request.GET.get('serial', None)
      if (serial):
         device = Object(serial)
         connected = device_is_online(device)
   context = {}
   context['connected'] = connected
   return HttpResponse(json.dumps(context), content_type="application/json") 

# TODO form only generates for devices.first()
@csrf_exempt
@login_required(login_url='/signin/')
def settings(request):
  context = {}
  user = request.user.id
  template = 'base/settings.html'
  if request.method == 'GET':

    if request.GET.get('device', False):
      devices = Device.objects.filter(owner=user)
      for device in devices:
        device.online = device_is_online(device)
      context['devices'] = devices
      template = 'base/settings_device_base.html'

    elif request.GET.get('account', False):
      form = SettingsForm(user=request.user)
      context['form'] = form
      user = User.objects.get(username=request.user)
      context['notification_choices'] = []
      for n in user.usersettings.interval_notification.all():
         context['notification_choices'].append(n.pk)
      template = 'base/settings_account.html'

    elif request.GET.get('dashboard', False):
      form = SettingsForm(user=request.user)
      context['form'] = form
      template = 'base/settings_dashboard.html'
    context['farmer_installed'] = 'farmer' in django_settings.INSTALLED_APPS
    return render(request, template, context)

@login_required(login_url='/signin/')
def settings_change_device(request):
   context = {}
   if request.method == 'GET':
      serial = request.GET.get('serial')
      if serial:
         user = User.objects.get(username = request.user)
         device = Device.objects.get(serial=serial)
         if device.owner == user:
            form = SettingsForm(device=device)
            context['device'] = device
            context['form'] = form
            context['utility_company_choices'] = []
            for c in device.devicesettings.utility_companies.all():
               context['utility_company_choices'].append(c.pk)
            context['rate_plan_choices'] = []
            for r in device.devicesettings.rate_plans.all():
               context['rate_plan_choices'].append(r.pk)
            context['territory_choices'] = []
            for t in device.devicesettings.territories.all():
               context['territory_choices'].append(t.pk)
   context['farmer_installed'] = 'farmer' in django_settings.INSTALLED_APPS
   return render(request, 'base/settings_device.html', context)
         

@login_required(login_url='/signin/')
def settings_account(request):
  context = {}
  errors = Set()
  user = User.objects.get(username = request.user)
  if request.method == 'POST':
    context['success'] = False
    form = SettingsForm(request.POST)
    notifications = request.POST.getlist('notifications', False)
    context['notifications'] = []
    if notifications:
      # disassociate all notifications
      user.usersettings.interval_notification.clear()
      for notification in notifications:
         # associate chosen notifications
         context['notifications'].append(notification)
         n = IntervalNotification.objects.get(pk=notification)
         user.usersettings.interval_notification.add(n)
         user.save()
      context['success'] = True

    elif form.is_valid():
      user = User.objects.get(username = request.user)
      new_username = form.cleaned_data['new_username']
      password1 = form.cleaned_data['password1']
      password2 = form.cleaned_data['password2']
      notifications = request.POST.get('notifications')
      template = 'base/settings_account.html'

      if new_username:
        try:
          user.username = new_username
          user.save()
          context['success'] = True
          context['username'] = user.username
        except IntegrityError:
          errors.add("Username taken")

      if password1 and password2:
        try:
          form.clean_password2()
          user.set_password(password1)
          user.save()
          auth_user = authenticate(username=request.user, password=password1)
          if user is not None:
            login(request, auth_user)
          context['success'] = True
        except forms.ValidationError, e:
          errors.add('; '.join(e.messages))
    
    context['errors'] = list(errors)
    return HttpResponse(json.dumps(context), content_type="application/json")
  else: return render(request, 'base/settings.html')

@login_required(login_url='/signin/')
def settings_device(request, serial):
  context = {}
  if request.method == 'POST':
    context['success'] = False
    user = User.objects.get(username=request.user)
    device = Device.objects.get(serial=serial)
    if device.owner == user:
       form = SettingsForm(request.POST)
       
       utility_company = request.POST.get('utility_company', False)
       context['utility_companies'] = []
       rate_plan = request.POST.get('rate_plans', False)
       context['rate_plan'] = []
       territory = request.POST.get('territories', False)
       context['territory'] = []
       if utility_company:
         # disassociate utility_company
         device.devicesettings.utility_companies.clear()
         # associate utility_company
         context['utility_companies'].append(utility_company)
         c = UtilityCompany.objects.get(pk=utility_company)
         device.devicesettings.utility_companies.add(c)
         device.save()
         context['success'] = True

       elif rate_plan:
        # disassociate utility_company
         device.devicesettings.rate_plans.clear()
         # associate utility_company
         context['rate_plan'].append(rate_plan)
         r = RatePlan.objects.get(pk=rate_plan)
         device.devicesettings.rate_plans.add(r)
         device.save()
         context['success'] = True
         
       elif territory:
         # disassociate utility_company
         device.devicesettings.territories.clear()
         # associate utility_company
         context['territory'].append(territory)
         t = Territory.objects.get(pk=territory)
         device.devicesettings.territories.add(t)
         device.save()
         context['success'] = True

       elif form.is_valid():
         user = User.objects.get(username = request.user)
         new_name = form.cleaned_data['new_name']
         utility_companies = form.cleaned_data['utility_companies']
         rate_plans = form.cleaned_data['rate_plans']
         territories = form.cleaned_data['territories']
         template = 'base/settings_device.html'
         device = Device.objects.get(serial=serial)

         if device.owner == user:
           if new_name:
             device.name = new_name
             device.save()
             context['new_name'] = device.name
             context['success'] = True       
    context['farmer_installed'] = 'farmer' in django_settings.INSTALLED_APPS
    return HttpResponse(json.dumps(context), content_type="application/json")
  else: return render(request, 'base/settings.html')

@login_required(login_url='/signin/')
def settings_dashboard(request):
  return render(request, 'base/settings.html')

@login_required(login_url='/signin/')
@csrf_exempt
def device_location(request, serial):
  context = {}
  context['success'] = False
  if request.method == 'POST':
    latitude = float(request.POST.get('latitude'))
    longitude = float(request.POST.get('longitude'))
    device = Device.objects.get(serial=serial)
    user = User.objects.get(username=request.user)
    if latitude and longitude and device.owner == user:
      device.position = Geoposition(latitude, longitude)
      device.save()
      context['success'] = True
  return HttpResponse(json.dumps(context), content_type="application/json")

@login_required(login_url='/signin/')
@csrf_exempt
def remove_device(request, serial):
  context = {}
  context['success'] = False
  if request.method == 'POST':
    user = User.objects.get(username=request.user)
    device = Device.objects.get(serial=serial)
    submitted_name = request.POST.get('submitted_name')
    if device.owner == user and submitted_name == device.name:
      device.delete()
      context['success'] = True
  return HttpResponse(json.dumps(context), content_type="application/json")














