# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0067_device_kilowatt_hours_daily'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='device',
            name='channel_0',
        ),
        migrations.AddField(
            model_name='device',
            name='channel_3',
            field=models.ForeignKey(related_name='Channel 2', blank=True, to='microdata.CircuitType', null=True),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='device',
            name='channel_1',
            field=models.ForeignKey(related_name='Channel 0', blank=True, to='microdata.CircuitType', null=True),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='device',
            name='channel_2',
            field=models.ForeignKey(related_name='Channel 1', blank=True, to='microdata.CircuitType', null=True),
            preserve_default=True,
        ),
    ]
