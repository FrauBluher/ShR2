# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0059_auto_20150430_0324'),
    ]

    operations = [
        migrations.AddField(
            model_name='event',
            name='frequency',
            field=models.IntegerField(default=1),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='event',
            name='start',
            field=models.IntegerField(default=1),
            preserve_default=False,
        ),
        migrations.AlterField(
            model_name='event',
            name='dataPoints',
            field=models.CharField(help_text=b'Expects a JSON encoded string of values:{timeStart(int, milliseconds),\n [    {wattage(float, optional),\n    current(float, optional),\n    voltage(float, optional),\n    appliance_pk(int, optional),\n    event_code(int, optional),\n    channel(int, optional)}\n,...]', max_length=1000),
            preserve_default=True,
        ),
    ]
