# Create your views here.
from django.http import HttpResponse, HttpResponseRedirect
from django.views.decorators.csrf import csrf_exempt
from microdata.models import Event, Device
from django import forms

import git

class DatagenForm(forms.Form):
   device = forms.ModelChoiceField(label='Device', queryset=Device.objects.all())

@csrf_exempt
def gitupdate(request):
    if request.method == 'POST':
        try:
            g = git.cmd.Git("/home/ubuntu/seads-git/ShR2/")
            g.pull()
            return HttpResponse(status=200)
        except: return HttpResponse(status=500)
    else: return HttpResponse(status=403)

@csrf_exempt
def echo(request):
   return HttpResponse(status=200)

@csrf_exempt
def echo_args(request, args):
   return HttpResponse(status=200)
   
def datagen(request):
   if request.method == 'POST':
      form = DatagenForm(requst.POST)
      if form.is_valid():
         return HttpResponseRedirect('/')
   else:
      form = DatagenForm()
   return render(request, 'datagen.html', {'form':form})
      