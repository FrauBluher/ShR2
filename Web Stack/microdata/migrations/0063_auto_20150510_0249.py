# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0062_device_share_with'),
    ]

    operations = [
        migrations.AlterField(
            model_name='event',
            name='dataPoints',
            field=models.CharField(help_text=b'Expects a JSON encoded string of values:{time(int, milliseconds),frequency(int, Hz),\n [    {wattage(float, optional),\n    current(float, optional),\n    voltage(float, optional),\n    appliance_pk(int, optional),\n    event_code(int, optional),\n    channel(int, optional)}\n ,...]', max_length=1000),
            preserve_default=True,
        ),
    ]
