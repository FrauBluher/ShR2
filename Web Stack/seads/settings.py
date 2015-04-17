"""
Django settings for seads project.

For more information on this file, see
https://docs.djangoproject.com/en/1.7/topics/settings/

For the full list of settings and their values, see
https://docs.djangoproject.com/en/1.7/ref/settings/
"""

from django.conf.global_settings import TEMPLATE_CONTEXT_PROCESSORS as TCP

# Build paths inside the project like this: os.path.join(BASE_DIR, ...)
import os
BASE_DIR = os.path.dirname(os.path.dirname(__file__))

# bad idea - move elsewhere
WUNDERGROUND_KEY = '24e5a1d55557f6d5'
OWM_KEY = 'b19ab21f2b0f046b8ec430fae9e28a84'
GOOGLE_API_KEY = 'AIzaSyAZvP6Z9ia_oCJjmFORgVyVTnQMkJMbFTw'

SES_EMAIL = 'seadsystems@gmail.com'
S3_BUCKET = 'elasticbeanstalk-us-west-2-062302560606'

ORG_NAME = 'SEADS'
BASE_URL = 'seads.brabsmit.com'

TEMPLATE_DIRS = (
    BASE_DIR + '/home/templates/',
    BASE_DIR + '/webapp/templates/',
    BASE_DIR + '/debug/html/',
)

TEMPLATE_CONTEXT_PROCESSORS = TCP + (
    'django.core.context_processors.request',
)

SUIT_CONFIG = {
    'ADMIN_NAME': 'SEADS Admin',
}

MEDIA_ROOT = BASE_DIR+'/webapp/static/media/'
MEDIA_URL = '/static/media/'

LOGIN_REDIRECT_URL = '/data/'

AUTHENTICATION_BACKENDS = ('django.contrib.auth.backends.ModelBackend',) 

STATICFILES_FINDERS = (
   'django.contrib.staticfiles.finders.FileSystemFinder',
   'django.contrib.staticfiles.finders.AppDirectoriesFinder'
)

STATIC_PATH = '/home/ubuntu/seads-git/ShR2/Web Stack/webapp/static/'

STATIC_ROOT = '/static/'

GEOIP_PATH = '/webapp/static/webapp/dat/'

REST_FRAMEWORK = {
    'DEFAULT_AUTHENTICATION_CLASSES': (
        'rest_framework.authentication.BasicAuthentication',
        'rest_framework.authentication.SessionAuthentication',
        'rest_framework.authentication.TokenAuthentication',
    ),
    #'DEFAULT_PERMISSION_CLASSES': (
    #    'rest_framework.permissions.IsAuthenticated',
    #),
}

# Quick-start development settings - unsuitable for production
# See https://docs.djangoproject.com/en/1.7/howto/deployment/checklist/

# SECURITY WARNING: keep the secret key used in production secret!
SECRET_KEY = 'n$ibk0^yobvqlfp2b*2(!rmvu0i3pzjyc0g=2^(crwl+(-rsot'

# SECURITY WARNING: don't run with debug turned on in production!
DEBUG = True

TEMPLATE_DEBUG = True

ALLOWED_HOSTS = []

GRAPH_MODELS = {
  'all_applications': True,
  'group_models': True,
}

# Application definition

INSTALLED_APPS = (
    'suit',
    'django.contrib.admin',
    'django.contrib.auth',
    'django.contrib.contenttypes',
    'django.contrib.sessions',
    'django.contrib.messages',
    'django.contrib.staticfiles',
    'django_extensions',
    'django.contrib.gis',
    'dbbackup',
    'restless',
    'rest_framework.authtoken',
    'rest_framework',
    'rest_framework_swagger',
    'gmapi',
    'geoposition',
    'paintstore',
    'microdata',
    'webapp',
    'home',
    'farmer',
    'debug',
)

MIDDLEWARE_CLASSES = (
    'django.contrib.sessions.middleware.SessionMiddleware',
    'django.middleware.common.CommonMiddleware',
    'django.middleware.csrf.CsrfViewMiddleware',
    'django.contrib.auth.middleware.AuthenticationMiddleware',
    'django.contrib.auth.middleware.SessionAuthenticationMiddleware',
    'django.contrib.messages.middleware.MessageMiddleware',
    'django.middleware.clickjacking.XFrameOptionsMiddleware',
)


ROOT_URLCONF = 'seads.urls'

WSGI_APPLICATION = 'seads.wsgi.application'


# Database
# https://docs.djangoproject.com/en/1.7/ref/settings/#databases

DATABASES = {
    'default': {
        'ENGINE': 'django.db.backends.sqlite3',
        'NAME': os.path.join(BASE_DIR, 'db.sqlite3'),
    }
}

# Internationalization
# https://docs.djangoproject.com/en/1.7/topics/i18n/

LANGUAGE_CODE = 'en-us'

TIME_ZONE = 'UTC'

USE_I18N = True

USE_L10N = True

USE_TZ = True


# Static files (CSS, JavaScript, Images)
# https://docs.djangoproject.com/en/1.7/howto/static-files/

STATIC_URL = '/static/'

