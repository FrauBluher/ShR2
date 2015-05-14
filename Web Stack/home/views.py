from django.shortcuts import render

# Create your views here.

from django.http import HttpResponseRedirect, HttpResponse
from django.contrib.auth.models import User
from rest_framework import viewsets
from home.serializers import UserSerializer
from django.shortcuts import render_to_response
from django.template import RequestContext
from django.contrib.auth import authenticate, login, logout
from django.contrib import messages
from django import forms
from django.contrib.auth.forms import UserCreationForm, AuthenticationForm
from rest_framework.authtoken.models import Token
from django.views.decorators.csrf import csrf_exempt

from webapp.models import UserSettings

import git


# ViewSets define the view behavior.
class UserViewSet(viewsets.ModelViewSet):
    queryset = User.objects.all()
    serializer_class = UserSerializer
    #lookup_field = 'username'

def account(request):
    if request.user.is_authenticated():
        return render_to_response('base/account.html', context_instance=RequestContext(request))
    else:
        return HttpResponseRedirect("/signin/")

def register(request):
    if request.user.is_authenticated():
        return HttpResponseRedirect("/data/")
    else:
        if request.method == 'POST':
            form = UserCreationForm(request.POST)
            if form.is_valid():
                new_user = form.save()
                username = form.cleaned_data.get('username')
                password = form.cleaned_data.get('password2')
                user = authenticate(username=username, password=password)
                login(request, user)
                UserSettings.objects.create(user=user)
                return HttpResponseRedirect("/data/")
        else:
            form = UserCreationForm()
        return render(request, "base/register.html", {
            'form': form,
        })

def signin(request):
    if request.method == 'POST':
        form = AuthenticationForm(data=request.POST)
        if form.is_valid():
            username = request.POST.get('username', '')
            password = request.POST.get('password', '')
            user = authenticate(username=username, password=password)
            if user is not None:
                login(request, user)
                token = Token.objects.get_or_create(user=user)
                context = {'api-auth-token':token}
                return HttpResponseRedirect("/data/")
    else:
        form = AuthenticationForm()
    return render(request, "base/signin.html", {
        'form': form,
    })

def signout(request):
    logout(request)
    return HttpResponseRedirect("/signin/")

def index(request):
    if request.user.is_authenticated():
        return HttpResponseRedirect("/data/")
    return render(request, "base/index.html")
