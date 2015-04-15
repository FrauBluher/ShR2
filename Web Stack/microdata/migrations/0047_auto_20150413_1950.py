# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0046_auto_20150413_1949'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='devicesettings',
            name='id',
        ),
        migrations.AddField(
            model_name='devicesettings',
            name='device_serial',
            field=models.IntegerField(default=1, unique=True, serialize=False, primary_key=True),
            preserve_default=False,
        ),
    ]
