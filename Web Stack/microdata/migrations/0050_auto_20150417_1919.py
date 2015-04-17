# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0049_auto_20150415_1645'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='devicesettings',
            name='device',
        ),
        migrations.DeleteModel(
            name='DeviceSettings',
        ),
    ]
