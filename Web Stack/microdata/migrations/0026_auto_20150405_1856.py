# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0025_auto_20150310_2153'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='event',
            name='appliance',
        ),
        migrations.RemoveField(
            model_name='event',
            name='current',
        ),
        migrations.RemoveField(
            model_name='event',
            name='event_code',
        ),
        migrations.RemoveField(
            model_name='event',
            name='timestamp',
        ),
        migrations.RemoveField(
            model_name='event',
            name='voltage',
        ),
        migrations.RemoveField(
            model_name='event',
            name='wattage',
        ),
        migrations.AddField(
            model_name='event',
            name='dataPoints',
            field=models.CharField(default='1', help_text=b'Expects a JSON encoded string of values:[   {timestamp(int),   wattage(float, optional),   current(float, optional),   voltage(float, optional),   appliance(float, optional),   event_code(int, optional)},...]', max_length=1000),
            preserve_default=False,
        ),
    ]
