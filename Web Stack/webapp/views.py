from django.shortcuts import render, render_to_response
from django.contrib.auth.decorators import login_required
from microdata.models import Device, Event, Appliance, Circuit, CircuitType
from django.views.decorators.csrf import csrf_exempt
from django.contrib.auth.models import User, Group
from django import forms
from django.http import HttpResponse
from django.conf import settings as django_settings
from sets import Set
from collections import defaultdict
from django.contrib.auth import authenticate, login
from webapp.models import EventNotification, IntervalNotification, UtilityCompany, RatePlan, Territory, DashboardSettings
from geoposition import Geoposition
from django.core.servers.basehttp import FileWrapper
import cStringIO as StringIO
from microdata.views import initiate_job_to_glacier
from webapp.models import Tier
from django.core import serializers


from django.db import IntegrityError

from datetime import datetime, timedelta
import json
import time
import re
from influxdb import client as influxdb
from calendar import monthrange

# Create your views here.

def make_choices(querysets):
  choices = []
  for queryset in querysets:
     for choice in queryset:
       choices.append((choice.pk, choice))
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
  first_name = forms.CharField(max_length=254,
                                min_length=1,
                                required=False,
                                widget=forms.TextInput(attrs={
                                    'class' : 'form-control input-md'}))
  last_name = forms.CharField(max_length=254,
                              min_length=1,
                              required=False,
                              widget=forms.TextInput(attrs={
                                'class' : 'form-control input-md'}))
  email = forms.EmailField(required=False,
                           widget=forms.TextInput(attrs={
                                'class' : 'form-control input-md'}))

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
  share_with_choices = []
  share_with = forms.ChoiceField(
      widget=forms.SelectMultiple(),
      choices=(
          share_with_choices,
      ),
  )
  new_name = forms.CharField(max_length=254,
                             min_length=1,
                             required=False,
                             widget=forms.TextInput(attrs={
                              'class' : 'form-control input-md'
                              })
                            )
  channel_1_choices = []
  channel_2_choices = []
  channel_3_choices = []
  channel_1 = forms.ChoiceField(
    widget=forms.Select(attrs={
          'class':'form-control',
          'id':'channel_1-dropdown'
          }
        ),
    choices=(
      channel_1_choices
    ),
    required=False
  )
  channel_2 = forms.ChoiceField(
    widget=forms.Select(attrs={
          'class':'form-control',
          'id':'channel_2-dropdown'
          }
        ),
    choices=(
      channel_2_choices
    ),
    required=False
  )
  channel_3 = forms.ChoiceField(
    widget=forms.Select(attrs={
          'class':'form-control',
          'id':'channel_3-dropdown'
          }
        ),
    choices=(
      channel_3_choices
    ),
    required=False
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
      self.notification_choices = make_choices([IntervalNotification.objects.all(), EventNotification.objects.all()])
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
      self.share_with_choices = make_choices([User.objects.all().exclude(username=device.owner.username),])
      self.channel_1_choies = make_choices([CircuitType.objects.all(),])
      self.channel_2_choies = make_choices([CircuitType.objects.all(),])
      self.channel_3_choies = make_choices([CircuitType.objects.all(),])

      self.fields['share_with'] = forms.ChoiceField(
        widget=forms.SelectMultiple(attrs={
          'class':'form-control',
          'id':'share_with_select'
          }
        ),
        choices=(
          self.share_with_choices
        ),
        required=False
      )
      
      self.fields['channel_1'] = forms.ChoiceField(
        widget=forms.Select(attrs={
          'class':'form-control',
          'id':'channel_1-dropdown'
          }
        ),
        choices=(
          self.channel_1_choies
        ),
        required=False
      )
      self.fields['channel_2'] = forms.ChoiceField(
        widget=forms.Select(attrs={
          'class':'form-control',
          'id':'channel_2-dropdown'
          }
        ),
        choices=(
          self.channel_2_choies
        ),
        required=False
      )
      self.fields['channel_3'] = forms.ChoiceField(
        widget=forms.Select(attrs={
          'class':'form-control',
          'id':'channel_3-dropdown'
          }
        ),
        choices=(
          self.channel_3_choies
        ),
        required=False
      )
      self.utility_company_choices = make_choices([UtilityCompany.objects.all(),])
      self.rate_plan_choices = make_choices([RatePlan.objects.all(),])
      self.territory_choices = make_choices([Territory.objects.all(),])

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
      return make_choices([user.usersettings.interval_notification.all(), user.usersettings.event_notification.all()])

  def get_utility_companies(self, *args, **kwargs):
    device = kwargs.pop('device', None)
    if device:
      return make_choices([device.devicewebsettings.utility_companies.all(),])

  def get_rate_plans(self, *args, **kwargs):
    device = kwargs.pop('device', None)
    if device:
      return make_choices([device.devicewebsettings.rate_plans.all(),])

  def get_territories(self, *args, **kwargs):
    device = kwargs.pop('device', None)
    if device:
      return make_choices([device.devicewebsettings.territories.all(),])

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
   """
      The first function called when a user has logged in and accesses the Dashboard.

      **Context**

         ``my_devices``

            A list of :class:`microdata.models.Device` objects that either belong to the user or are shared with the user.
            This list is used to populate the sidebar with :class:`microdata.models.Device` links.

      **Templates:**

         `base/dashboard.html`

   """
   user = request.user.id
   my_devices = Device.objects.filter(owner=user)
   my_devices = my_devices | Device.objects.filter(share_with=user)
   context = {'my_devices': my_devices}
   return render(request, 'base/landing.html', context)


@login_required(login_url='/signin/')
def dashboard(request):
   """
      The first function called when a user has logged in and accesses the Dashboard.

      **Context**

         ``my_devices``

            A list of :class:`microdata.models.Device` that either belong to the user or are shared with the user.
            This list is used to generate markers on the graph.


         ``server_time``

            The current time as reported by the server. This is used as a time offset for the graph.


      **Templates:**

         `base/dashboard.html`

   """
   user = request.user.id
   if request.user.is_authenticated():
      my_devices = Device.objects.filter(owner=user)
      my_devices = my_devices | Device.objects.filter(share_with=user)
   else: my_devices = None
   db = influxdb.InfluxDBClient(django_settings.INFLUXDB_URI,8086,'root','root','seads')
   for device in my_devices:
      device.circuits = []
      device.circuits.append(device.channel_1)
      device.circuits.append(device.channel_2)
      device.circuits.append(device.channel_3)
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


def group_by_mean(serial, unit, start, stop, localtime, circuit_pk):
   query = ''
   if unit == 'y': unit = 'm'
   if (start == ''): start = 'now() - 1d'
   else: start = '\''+datetime.fromtimestamp(start).strftime('%Y-%m-%d %H:%M:%S')+'\''
   if (stop == ''): stop = 'now()'
   else: stop = '\''+datetime.fromtimestamp(stop+25200).strftime('%Y-%m-%d %H:%M:%S')+'\''
   db = influxdb.InfluxDBClient(django_settings.INFLUXDB_URI,8086,'root','root','seads')
   result = db.query('show series')[0]
   channels = Set()
   for series in result['points']:
      rg = re.compile('device.'+str(serial))
      if re.match(rg, series[1]):
         channel_pk = series[1].split('device.'+str(serial)+'.')
         if (len(channel_pk) < 2): continue
         else: channels.add(channel_pk[-1])
   mean = {}
   to_merge = []
   for circuit in channels:
      # Specifying a circuit_pk filters the query based on circuits on the device.
      query = 'select * from 1'+unit+'.device.'+str(serial)+'.'+str(circuit)+' where time > '+start+' and time < '+stop
      if circuit_pk:
        query = 'select * from 1'+unit+'.device.'+str(serial)+'.'+str(circuit_pk)+' where time > '+start+' and time < '+stop
      group = []
      try:
        group = db.query(query)
      except:
        pass
      if (len(group)): group = group[0]['points']
      # else: return None
      # hack. Remove sequence_number and timezone offset for GMT
      new_group = []
      for s in group:
         # use 28800 for daylight savings (-8 PDT)
         # use 25200 for normal (-7 PDT)
         #offset = time.time() - localtime
         s[0] = int(round(s[0]-25200))
         if len(str(s[0])) == 13:
            s[0] = s[0]/1000
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
           'dataLimitTo': data[0][0],
           'query': query
          }
   return data

@login_required(login_url='/signin/')
def default_chart(request):
   """
      The first function called when a user has logged in and accesses the Dashboard.

      **Context**

         ``my_devices``

            A list of :class:`microdata.models.Device` that either belong to the user or are shared with the user.
            This list is used to generate markers on the graph.


         ``server_time``

            The current time as reported by the server. This is used as a time offset for the graph.


      **Templates:**

      `   base/dashboard.html`

   """
   if request.method == 'GET':
      user = User.objects.get(username=request.user)
      devices = Device.objects.filter(owner=user)
      devices = devices | Device.objects.filter(share_with=user)
      db = influxdb.InfluxDBClient(django_settings.INFLUXDB_URI,8086,'root','root','seads')
      for device in devices:
        device.circuits = []
        device.circuits.append(device.channel_1)
        device.circuits.append(device.channel_2)
        device.circuits.append(device.channel_3)
      context = {'my_devices': devices,
                 'server_time': time.time()*1000,
                }
      return render(request, 'base/dashboard.html', context)

@login_required(login_url='/signin/')
def generate_average_wattage_usage(request, serial):
    """
        Get the cost today, average wattage, and current wattage associated with the specified device.
        Should be called through AJAX and is expected to take a couple seconds to complete.

        **Context**

            ``cost_today``

                A simple lookup from the device itself. This could be transformed to a template
                variable instead in the future.


            ``average_wattage``

                This is done by simply selecting the newest mean value from the 1 day series. This
                is probably cheating but it's really fast compared to computing a new average
                at every lookup.


            ``current_wattage``

                This is simply the sum of the newest wattages from each channel of the device.
    """
    
    context = {}
    user = User.objects.get(username=request.user)
    device = Device.objects.get(serial=serial)
    if device.owner == user or user in device.share_with.all():
      db = influxdb.InfluxDBClient(django_settings.INFLUXDB_URI,8086,'root','root','seads')
      result = db.query('show series')[0]
      channels = Set()
      for series in result['points']:
         rg = re.compile('device.'+str(device.serial))
         if re.match(rg, series[1]):
            channel = series[1].split('device.'+str(device.serial)+'.')
            if (len(channel) < 2): continue
            else:
               channels.add(channel[-1])
      average_wattage = 0
      try:
        average_wattage = db.query('select mean from 1d.device.'+str(device.serial)+' limit 1')[0]['points'][0][2]
      except:
        pass
      current_wattage = 0
      cost_today = device.cost_daily
      for circuit in channels:
         try:
            this_wattage = db.query('select * from 1m.device.'+str(device.serial)+'.'+str(circuit)+' limit 1')[0]['points'][0]
            if this_wattage[0] > time.time() - 1000:
               current_wattage += this_wattage[2]
         except:
            pass
      context['cost_today'] = float("{0:.2f}".format(cost_today))
      context['average_wattage'] = int(average_wattage)
      context['current_wattage'] = int(current_wattage)
      return context
      
@login_required(login_url='/signin/')
def get_wattage_usage(request):
   """
   Wrapper function that calls `webapp.views.generate_average_wattage_usage`.
   """
   
   context = {}
   if request.method == 'GET':
       serial = request.GET.get('serial')
       context = generate_average_wattage_usage(request, serial)
   return HttpResponse(json.dumps(context), content_type="application/json")

   
@login_required(login_url='/signin/')
def device_data(request, serial):
   """
       Get the cost today, average wattage, and current wattage associated with the specified device.
       Should be called through AJAX and is expected to take a couple seconds to complete.

       **Context**

           ``cost_today``

               A simple lookup from the device itself. This could be transformed to a template
               variable instead in the future.


           ``average_wattage``

               This is done by simply selecting the newest mean value from the 1 day series. This
               is probably cheating but it's really fast compared to computing a new average
               at every lookup.


           ``current_wattage``

               This is simply the sum of the newest wattages from each channel of the device.
   """
   
   context = {}
   if request.method == 'GET':
      user = User.objects.get(username=request.user)
      device = Device.objects.get(serial=serial)
      localtime = int(float(request.GET.get('localtime', time.time())))
      chart_load = request.GET.get('chart_load', False)
      if device.owner == user or user in device.share_with.all():
         unit = request.GET.get('unit','')
         start = int(float(request.GET.get('from','')))
         stop = int(float(request.GET.get('to','')))
         if chart_load: stop += 25200
         if start < stop: start += 25200
         circuit_pk = request.GET.get('circuit_pk')
         context = group_by_mean(serial,unit,start,stop,localtime,circuit_pk)
   return HttpResponse(json.dumps(context), content_type="application/json")


@login_required(login_url='/signin/')
def device_chart(request, serial):
   """
       Return an HTML/Javascript chart ready to be rendered to the client.
       No data is preloaded. However, a chart will have a unique endpoint
       to retrieve data based on device serial.

       **Context**

           ``device``

               A :class:`microdata.models.Device` object for the serial provided.


           ``circuit_pk``

               If a ``circuit_pk`` is specified, then this function will return a chart
               with just the circuit specified by the primary key. This returns the pk.
               
            ``circuit_name``
            
                If a ``circuit_pk`` is specified, then this function will return a chart
                with just the circuit specified by the primary key. This returns the name.


           ``circuits``

               A list of circuits that relate to the channels of the device.
               
           ``server_time``

              The time as seen by the server right now. Used to synchronize the chart.

           ``stack``

               This is used to specify whether the chart is stacked or unstacked. This
               was intended to be a setting that a user could toggle via the settings page.

      **Template**
 
         `base/chart.html`

   """
   context = {}
   if request.method == 'GET':
      circuit_pk = request.GET.get('circuit_pk')
      stack = request.GET.get('stack')
      user = User.objects.get(username=request.user)
      device = Device.objects.get(serial=serial)

      if device.owner == user or user in device.share_with.all():
         context['device'] = device

         channel_names = Set()
         if circuit_pk:
            circuit = CircuitType.objects.get(pk=circuit_pk)
            channel_names = [circuit.name]
            context['circuit_pk'] = circuit_pk
            context['circuit_name'] = circuit.name
         else:
             db = influxdb.InfluxDBClient(django_settings.INFLUXDB_URI,8086,'root','root','seads')
             result = db.query('show series')[0]
             for series in result['points']:
                rg = re.compile('device.'+str(device.serial))
                if re.match(rg, series[1]):
                   circuit_pk = series[1].split('device.'+str(device.serial)+'.')
                   if (len(circuit_pk) < 2): continue
                   else: channel_names.add(circuit_pk[-1])

         context['circuits'] = []
         for circuit in channel_names:
            try:
               circuit = CircuitType.objects.get(pk=circuit)
            except:
               pass
            context['circuits'].append(circuit);

         context['server_time'] = time.time()*1000
         context['stack'] = True #stack == 'true'
   return render(request, 'base/chart.html', context)
   

@login_required(login_url='/signin/')
def charts_deprecated(request, serial, unit):
   """
   DEPRECATED
   """
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
    db = influxdb.InfluxDBClient(django_settings.INFLUXDB_URI,8086,'root','root','seads')
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
   """
       Check to see if a device is connected to the system or not.
       
       This is done by checking to see how recent the newest data point in
       the database is. If it's not older than 40 seconds, assume the device
       is currently connected.

       **Context**

           ``connected``

               Boolean. True if device has data not older than 40 seconds.

   """
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
  """
      The main function to handle settings requests. This module interprets
      where on the settings page the user is and provides correct context
      corresponding to their state.
      
      There are three possible states:
      
      1. Device

      **Context**
      
        ``devices``
        
            A list of devices the user owns. These are the devices
            this user is allowed to modify in a restricted way.
      
      **Template**
      
            `base/settings_device_base.html`
      
      2. Account
      
      **Context**
      
        ``form``
        
            A :class:`webapp.views.SettingsForm` object that provides the
            fields to a user necessary to make custom modifications to their
            account.
            
        ``notification_choices``
        
            A confirmation of the notification choices the user is
            subscribed to. Used for client side verification.
            
      **Template**
      
        `base/settings_account.html`
      
      3. Dashboard
      
      **Context**
      
        ``form``
        
            A :class:`webapp.views.SettingsForm` object that provides the
            fields to a user necessary to make custom modifications to their
            dashboard.
      
      **Template**
      
        `base/settings_dashboard.html`

  """
  context = {}
  user = request.user.id
  template = 'base/settings.html'
  if request.method == 'GET':
    goto = request.GET.get('goto')
    if goto == 'device':
       context['goto'] = 'device'
       return render(request, template, context)
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
   """
       This class is called when a user is ready to modify a device.
       Since there are custom fields being displayed, some extra context
       must be returned to properly serve the form.

       **Context**

           ``device``
           
              The :class:`microdata.models.Device` being modified
              
           ``form``
           
              A :class:`webapp.views.SettingsForm` with values populated
              by specifying a device.
              
           ``utility_company_choices`` ``rate_plan_choices`` ``territory_choices``
           
              The various checkbox/dropdown choices for a user to customize
              the device's settings.

           ``farmer_installed``
           
              A proof-of-concept variable that will enable certain
              functionality on the HTML page if this application is installed.
              
        **Template**
        
           `base/settings_device.html`

   """
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
            for c in device.devicewebsettings.utility_companies.all():
               context['utility_company_choices'].append(c.pk)
            context['rate_plan_choices'] = []
            for r in device.devicewebsettings.rate_plans.all():
               context['rate_plan_choices'].append(r.pk)
            context['territory_choices'] = []
            for t in device.devicewebsettings.territories.all():
               context['territory_choices'].append(t.pk)
   context['farmer_installed'] = 'farmer' in django_settings.INSTALLED_APPS
   return render(request, 'base/settings_device.html', context)
         

@login_required(login_url='/signin/')
def settings_account(request):
  """
      This class deals with the actual modification of an account
      via the account settings. 

      **Context**

          ``errors``

             A list of errors that were encountered while processing the
             request. This is used to present to the user if one of their
             actions did not validate.

          ``success``

             Boolean specifying whether or not the action was successful.
             This would be false if a user's action did not verify.

          ``notifications``

             A list of notifications that a user is subscribed to
             after updating the list.

          ``username``

             If the user opted to change their username, then the new
             username verified by the system is returned to update the
             HTML.

          ``first_name``

             If the user opted to change their first name, then the new
             first name verified by the system is returned to update the
             HTML.

          ``last_name``

             If the user opted to change their last name, then the new
             last name verified by the system is returned to update the
             HTML.

       **Template**

          `base/settings_account.html`

  """
  context = {}
  errors = Set()
  user = User.objects.get(username = request.user)
  if request.method == 'POST':
    sandbox_group = Group.objects.get(name='Sandbox')
    if sandbox_group in user.groups.all():
      context['errors'] = 'Sandbox user unable to change settings'
      return HttpResponse(json.dumps(context), content_type="application/json")
    context['success'] = False
    form = SettingsForm(request.POST)
    notifications = request.POST.getlist('notifications', False)
    context['notifications'] = []
    if notifications:
      # disassociate all notifications
      user.usersettings.interval_notification.clear()
      user.usersettings.event_notification.clear()
      for notification in notifications:
         # associate chosen notifications
         context['notifications'].append(notification)
         n = IntervalNotification.objects.filter(pk=notification).first()
         if n:
            user.usersettings.interval_notification.add(n)
         else:
            n = EventNotification.objects.filter(pk=notification).first()
            user.usersettings.event_notification.add(n)
         user.save()
      context['success'] = True

    elif form.is_valid():
      user = User.objects.get(username = request.user)
      new_username = form.cleaned_data['new_username']
      password1 = form.cleaned_data['password1']
      password2 = form.cleaned_data['password2']
      first_name = form.cleaned_data['first_name']
      last_name = form.cleaned_data['last_name']
      email = form.cleaned_data['email']
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
      if first_name and last_name:
          user.first_name = first_name
          user.last_name = last_name
          user.save()
          context['success'] = True
          context['first_name'] = user.first_name
          context['last_name'] = user.last_name
      if email:
          user.email = email
          user.save()
          context['success'] = True
    
    context['errors'] = list(errors)
    return HttpResponse(json.dumps(context), content_type="application/json")
  else: return render(request, 'base/settings.html')

@login_required(login_url='/signin/')
@csrf_exempt
def settings_device(request, serial):
  """
      This class deals with the actual modification of a device
      via the device settings. 

      **Context**

          ``success``

             Boolean specifying whether or not the action was successful.
             This would be false if a user's action did not verify.

          ``appliances``

             This is a list of appliance that the user requests to be
             associated with their custom circuit.

          ``utility_companies`` ``rate_plan`` ``territory``

             If the user opted to alter any of these fields,
             we make the alterations if they are verified
             and return the response for client side verification.

       **Template**

          `base/settings_device.html`

  """
  context = {}
  if request.method == 'POST':
    context['success'] = False
    user = User.objects.get(username=request.user)
    device = Device.objects.get(serial=serial)
    if device.owner == user:
       form = SettingsForm(request.POST)
       
       new_name = request.POST.get('new_name')
       
       if new_name:
         device.name = new_name
         device.save()
         context['success'] = True
         device_obj = {
           'name': device.name,
           'serial': device.serial
         }
         context['device'] = device_obj
       
       custom_channel = request.POST.get('custom_channel', False)
       appliance = request.POST.get('appliance', False)
       add = request.POST.get('add', None)
       if custom_channel and appliance:
         device.channel_1 = CircuitType.objects.get(pk=6)
         if add is True:
            device.channel_1.appliances.add(Appliance.objects.get(pk=int(appliance)))
         elif add is False:
            device.channel_1.appliances.remove(Appliance.objects.get(pk=int(appliance)))
         device.save()
         context['success'] = True
         return HttpResponse(json.dumps(context), content_type="application/json")
       channel_1 = request.POST.get('channel_1', False)
       channel_2 = request.POST.get('channel_2', False)
       channel_3 = request.POST.get('channel_3', False)
       context['appliances'] = {}
       if channel_1:
         device.channel_1 = CircuitType.objects.get(pk=channel_1)
         device.save()
         context['appliances']['channel_1'] = [x.name for x in device.channel_1.appliances.all()]
         context['success'] = True
       if channel_2:
         device.channel_2 = CircuitType.objects.get(pk=channel_2)
         device.save()
         context['appliances']['channel_2'] = [x.name for x in device.channel_2.appliances.all()]
         context['success'] = True
       if channel_3:
         device.channel_3 = CircuitType.objects.get(pk=channel_3)
         device.save()
         context['appliances']['channel_3'] = [x.name for x in device.channel_3.appliances.all()]
         context['success'] = True
       
       utility_company = request.POST.get('utility_company', False)
       context['utility_companies'] = []
       rate_plan = request.POST.get('rate_plans', False)
       context['rate_plan'] = []
       territory = request.POST.get('territories', False)
       context['territory'] = []
       share_with = request.POST.getlist('share_with')
       if utility_company:
         # disassociate utility_company
         device.devicewebsettings.utility_companies.clear()
         # associate utility_company
         context['utility_companies'].append(utility_company)
         c = UtilityCompany.objects.get(pk=utility_company)
         device.devicewebsettings.utility_companies.add(c)
         device.save()
         context['success'] = True

       elif rate_plan:
        # disassociate utility_company
         device.devicewebsettings.rate_plans.clear()
         # associate utility_company
         context['rate_plan'].append(rate_plan)
         r = RatePlan.objects.get(pk=rate_plan)
         device.devicewebsettings.rate_plans.add(r)
         device.save()
         context['success'] = True
         
       elif territory:
         # disassociate utility_company
         device.devicewebsettings.territories.clear()
         # associate utility_company
         context['territory'].append(territory)
         t = Territory.objects.get(pk=territory)
         device.devicewebsettings.territories.add(t)
         device.save()
         context['success'] = True

       elif share_with:
         for u in device.share_with.all():
            device.share_with.remove(u)
         for u in share_with:
            u = User.objects.get(pk=u)
            device.share_with.add(u)
         device.save()
         context['success'] = True

       elif form.is_valid():
         user = User.objects.get(username = request.user)
         new_name = form.cleaned_data['new_name']
         share_with = form.cleaned_data['share_with']
         utility_companies = form.cleaned_data['utility_companies']
         rate_plans = form.cleaned_data['rate_plans']
         territories = form.cleaned_data['territories']
         template = 'base/settings_device.html'
         device = Device.objects.get(serial=serial)

         if device.owner == user:
           if new_name:
             device.name = new_name
             device.save()
           if share_with:
             for u in device.share_with.all():
                device.share_with.remove(u)
             for u in share_with:
                u = User.objects.get(pk=u)
                device.share_with.add(u)
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

@login_required(login_url='/signin/')
@csrf_exempt
def export_data(request):
  context = {}
  context['success'] = False
  if request.method == 'GET':
    serial = request.GET.get('serial')
    user = User.objects.get(username=request.user)
    device = Device.objects.get(serial=serial)
    start = request.GET.get('start', 0)
    end = request.GET.get('end', 0)
    status_code = 200
    if datetime.fromtimestamp(int(start)) < (datetime.now() - timedelta(days=31*device.data_retention_policy)):
      # data has been moved to glacier. Start glacier job.
      status_code = initiate_job_to_glacier(request, user, int(end))
      return render(request, 'base/glacier_status.html', {'glacier':True})
    elif device.owner == user:
      try:
        db = influxdb.InfluxDBClient(django_settings.INFLUXDB_URI,8086,'root','root','seads')
        if start is not 0 and end is not 0:
          data = db.query('select * from device.'+str(device.serial)+' where time > '+start+'s and time < '+end+'s')
        else:
          data = db.query('select * from device.'+str(device.serial))
      except:
        pass
    response = HttpResponse(content_type='text/plain')
    response['Content-Disposition'] = 'attachment; filename="export.txt"'
    response.write(data)
    return response

@login_required(login_url='/signin/')
def billing_information(request):
   """
       Dashboard function called via AJAX.

       This gives the user live information about their bill status
       as it stands from within the server's calculations. The current
       tier is returned as well as the total KWh consumed for this month.

       **Context**

           ``device``

              The :class:`microdata.models.Device` object that the
              request was completed for.

           ``tier_progress_list``

              A tuple containing a :class:`microdata.models.Device`
              object as well as a percentage of the progess the
              :class:`microdata.models.Device` is to reaching the next
              :class:`webapp.models.Tier` level.

           ``territory``

               This returns the current :class:`webapp.models.Territory`
               which assists in client-side calculations.
               
            ``tiers``

               A list of all the :class:`webapp.models.Tier` objects
               associated with the chosen :class:`webapp.models.RatePlan`.

        **Template**

           `base/billing_information.html.html`

   """
   context = {}
   if request.method == 'GET':
      serial = request.GET.get('serial')
      db = influxdb.InfluxDBClient(django_settings.INFLUXDB_URI,8086,'root','root','seads')
      user = User.objects.get(username=request.user)
      context['tier_progress_list'] = []
      device_tier_progress = {}
      this_month = datetime.now().month
      this_year = datetime.now().year
      days_this_month = monthrange(this_year,this_month)[1]
      for device in Device.objects.filter(owner=user) | Device.objects.filter(share_with=user):
         current_tier_level = 1
         try:
            current_tier_level = db.query('select * from tier.device.'+str(device.serial)+' limit 1')[0]['points'][0][2]
         except:
            pass
         device.device_tiers = Tier.objects.filter(rate_plan=device.devicewebsettings.rate_plans.all()[0])
         progress = 0.0
         if device.devicewebsettings.current_tier.max_percentage_of_baseline != None:
            territory = device.devicewebsettings.territories.all()[0]
            if (this_month >= territory.summer_start and this_month < territory.winter_start ):
                progress = device.kilowatt_hours_monthly/( device.devicewebsettings.current_tier.max_percentage_of_baseline/100*territory.summer_rate*days_this_month)*100
            else:
                progress = device.kilowatt_hours_monthly/( device.devicewebsettings.current_tier.max_percentage_of_baseline/100*territory.winter_rate*days_this_month)*100
            if progress > 100: progress = 100
         context['tier_progress_list'].append({'device':device, 'progress':progress});
         if device.serial == int(serial):
            context['current_device'] = {'device':device, 'progress':progress}
         device_tier_progress[device.serial] = progress
      device = Device.objects.get(serial=int(serial))
      if device.owner == user or user in device.share_with.all():
         context['territory'] = territory
         context['tiers'] = Tier.objects.filter(rate_plan=device.devicewebsettings.rate_plans.all()[0])
         context['device'] = device
         return render(request, 'base/billing_information.html', context)
   return render(status_code=400)
   
@login_required(login_url='/signin/')
def circuits_information(request):
   """
       Dashboard function called via AJAX.

       This function provides the data necessary to render a simple
       donut chart showing the breakdown of a circuit by the amount
       of energy it has consumed relative to other circuits.

       **Context**

           ``circuits``

              A list of tuples of the form::
              
                  [
                      microdata.models.Device.channel_1,
                      channel_1_kwh
                  ]

        **Template**

           `base/base/circuits_information.html`

   """
   if request.method == 'GET':
      serial = request.GET.get('serial')
      device = Device.objects.get(serial=serial)
      db = influxdb.InfluxDBClient(django_settings.INFLUXDB_URI,8086,'root','root','seads')
      user = User.objects.get(username=request.user)
      if device.owner == user or user in device.share_with.all():
         kilowatt_hours_monthly = device.kilowatt_hours_monthly
         channel_1_kwh = 0
         channel_2_kwh = 0
         channel_3_kwh = 0
         try:
           channel_1_kwh = db.query('select sum(wattage) from device.'+str(serial)+'.'+str(device.channel_1.pk)+' where time > now() - 1M')[0]['points'][0][1]
         except: pass
         try:
           channel_2_kwh = db.query('select sum(wattage) from device.'+str(serial)+'.'+str(device.channel_2.pk)+' where time > now() - 1M')[0]['points'][0][1]
         except: pass
         try:
           channel_3_kwh = db.query('select sum(wattage) from device.'+str(serial)+'.'+str(device.channel_3.pk)+' where time > now() - 1M')[0]['points'][0][1]
         except: pass
         context = {
            'circuits': [
               [device.channel_1, channel_1_kwh],
               [device.channel_2, channel_2_kwh],
               [device.channel_3, channel_3_kwh],
            ],
            'kilowatt_hours_monthly': kilowatt_hours_monthly
         }
         return render(request, 'base/circuits_information.html', context)
   return render(status_code=400)

def generate_heatmap_data(serial):
   db = influxdb.InfluxDBClient(django_settings.INFLUXDB_URI,8086,'root','root','seads')
   start_date = datetime.now()
   day_count = 365
   for single_date in (start_date - timedelta(n) for n in range(day_count)):
      print single_date
   

@login_required(login_url='/signin/')
def heatmap(request):
   context = {}
   if request.method == 'GET':
      serial = request.GET.get('serial')
      device = Device.objects.get(serial=serial)
      user = User.objects.get(username=request.user)
      if device.owner == user or user in device.share_with.all():
         context = generate_heatmap_data(serial)
         #[timestamp, total, min, max]
         now = int(time.time())
         context['data_points'] = [[now,0,-1,1],[now+1,0,-2,2],[now+2,0,-3,3],[now+3,0,-4,4]]
         context['dataLimitFrom'] = now
         context['dataLimitTo'] = now+4
         return render(request, 'base/heatmap.html', context)
   return render(status_code=400)
   
@login_required(login_url='/signin/')
def dashboard_update(request):
    context = {}
    if request.method == 'GET':
        serial = request.GET.get('current_device')
        user = User.objects.get(username=request.user)
        device = Device.objects.get(serial=serial)
        context['average_usage'] = generate_average_wattage_usage(request, device.serial)
    return HttpResponse(json.dumps(context), content_type="application/json")






