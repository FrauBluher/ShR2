from django.shortcuts import render
from django.shortcuts import render_to_response
from django.db.models import Q
from django.contrib.auth.decorators import login_required
from django.contrib.auth import get_user_model
from microdata.models import Device, Event, Appliance
from django.views.decorators.csrf import csrf_exempt
from django.contrib.auth.models import User
from django import forms
from django.http import HttpResponse
from django.conf import settings
from sets import Set
from collections import defaultdict
from urllib2 import Request, urlopen, URLError
from pyzipcode import ZipCodeDatabase

from datetime import datetime
import json
import time
import timeseries as ts
import re
from influxdb import client as influxdb

# Create your views here.

# UserForm is a customized version of an AuthenticationForm
class UserForm(forms.Form):
  new_username = forms.CharField(max_length=254,
                                 min_length=1,
                                 required=False,
                                 widget=forms.TextInput(attrs={
                                    'class' : 'form-control input-md'}))
  error_messages = {
    'password_mismatch': ("The two password fields didn't match."),
  }
  choices = [
    ("1", "Don't send any email"),
    ("2", "Weekly consumption details"),
    ("3", "Monthly consumption details"),
    ("4", "When we detect irregular household consumption"),
    ("5", "When we detect an irregular device"),
  ]
  password1 = forms.CharField(label=("Password"),
      widget=forms.PasswordInput(attrs={'class' : 'form-control input-md'}),
      required=False)
  password2 = forms.CharField(label=("Password confirmation"),
      widget=forms.PasswordInput(attrs={'class' : 'form-control input-md'}),
      help_text=("Enter the same password as above, for verification."),
      required=False)
  notifications = forms.ChoiceField(
      widget=forms.CheckboxSelectMultiple(),
      # Default choices
      choices=(
         choices,
        ),
      required=False
  )

  def __init__(self, *args, **kwargs):
    user = kwargs.pop('user', None)
    super(UserForm, self).__init__(*args, **kwargs)
    if user:
      self.choices = []
      i = 1
      for choice in user.usersettings.notifications.all():
        self.choices.append((str(i), choice))
        i += 1
      self.fields['notifications'] = forms.ChoiceField(
        widget=forms.CheckboxSelectMultiple(),
        choices=(
           self.choices
          ),
        required=False
  )

  def clean_password2(self):
    password1 = self.cleaned_data.get("password1")
    password2 = self.cleaned_data.get("password2")
    if password1 and password2 and password1 != password2:
        return None
    return password2

class DeviceForm(forms.Form):
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
  utility_companies = forms.ModelChoiceField(
    utility_company_choices,
    required=False)
  rate_plans = forms.ModelChoiceField(
    rate_plan_choices,
    required=False)
  territories = forms.ModelChoiceField(
    territory_choices,
    required=False)

  def __init__(self, *args, **kwargs):
    device = kwargs.pop('device', None)
    super(DeviceForm, self).__init(*args, **kwargs)
    if device:
      self.utility_company_choices = device.devicesettings.utility_companies.all()
      self.rate_plan_choices = device.devicesettings.rate_plans.all()
      self.territory_choices = device.devicesettings.territories.all()
      utility_companies = forms.ModelChoiceField(
        utility_company_choices,
        required=False)
      rate_plans = forms.ModelChoiceField(
        rate_plan_choices,
        required=False)
      territories = forms.ModelChoiceField(
        territory_choices,
        required=False)

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
   device = my_devices[0] if my_devices else None
   result = []
   if (device != None):
      db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
      result = db.query('select * from "'+str(device.serial)+'" limit 1;')[0]
   context = {'my_devices': my_devices,
              'appliances': result['columns'][2:],
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


def group_by_mean(serial, unit, start, stop):
   queries = []
   if (unit == 'y'): unit = 'm'
   if (start == ''): start = 'now() - 1d'
   else: start = '\''+datetime.datetime.fromtimestamp(int(start)).strftime('%Y-%m-%d %H:%M:%S')+'\''
   if (stop == ''): stop = 'now()'
   else: stop = '\''+datetime.datetime.fromtimestamp(int(stop)).strftime('%Y-%m-%d %H:%M:%S')+'\''
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
      group = db.query(query)
      queries.append(query)
      if (len(group)): group = group[0]['points']
      #else: return None
      # hack. Remove sequence_number and timezone offset for GMT
      new_group = []
      for s in group:
         s = [s[0],s[2]]
         new_group.append(s)
      to_merge += new_group
   data = merge_subs(to_merge)
   for point in data:
      point.insert(1, sum(point[1:]))
   if (len(data) < 1): data.append([None,])
   data = {'data': data,
           'unit': unit,
           'dataLimitFrom': data[len(data)-1][0],
           'queries': queries,
          }
   return data


@login_required(login_url='/signin/')
def default_chart(request):
   if request.method == 'GET':
      context = {}
      if (device):
         db = influxdb.InfluxDBClient('localhost',8086,'root','root','seads')
         result = db.query('list series')[0]
         appliances = Set()
         for series in result['points']:
            rg = re.compile('device.'+str(device.serial))
            if re.match(rg, series[1]):
               appliance = series[1].split('device.'+str(device.serial)+'.')
               if (len(appliance) < 2): continue
               else: appliances.add(appliance[-1])
         context = {'my_devices': [device],
                    'appliances': appliances,
                    'server_time': time.time()*1000,
                    }
      return render(request, 'base/dashboard.html', context)


@login_required(login_url='/signin/')
def charts(request, serial):
   if request.method == 'GET':
      unit = request.GET.get('unit','')
      start = request.GET.get('from','')
      stop = request.GET.get('to','')
      return HttpResponse(json.dumps(group_by_mean(serial,unit,start,stop)), content_type="application/json")


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
    return int(time.time()) - int(result[0]) < 10

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
      template = 'base/settings_device.html'

    elif request.GET.get('account', False):
      context['form'] = UserForm(user=request.user)
      template = 'base/settings_account.html'

    elif request.GET.get('dashboard', False):
      template = 'base/settings_dashboard.html'
    return render(request, template, context)


  elif request.method == 'POST':
    context['success'] = False

    group = request.POST.get('group')
    if group == 'device':
      
      context['success'] = True

    else:
      template = 'base/settings_account.html'
      form = UserForm(request.POST, user=None)
      if form.is_valid() or request.POST.get('notifications', False):
        user = User.objects.get(username = request.user)
        new_username = form.cleaned_data['new_username']
        password1 = form.cleaned_data['password1']
        password2 = form.cleaned_data['password2']
        notifications = request.POST.get('notifications')
        if new_username:
          user.username = new_username
          user.save()
          context['success'] = True
          context['username'] = user.username
        elif password1 and password2 and form.clean_password2():
          user.set_password(password1)
          user.save()
          context['success'] = True
        elif notifications:
          context['success'] = True
    return HttpResponse(json.dumps(context), content_type="application/json")
  else: return render(request, 'base/settings.html')

      
