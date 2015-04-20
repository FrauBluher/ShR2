# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0047_auto_20150413_1950'),
    ]

    operations = [
        migrations.AlterField(
            model_name='devicesettings',
            name='device_serial',
            field=models.IntegerField(unique=True, serialize=False, editable=False, primary_key=True),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='devicesettings',
            name='main_channel',
            field=models.CharField(default=1, max_length=1, choices=[(1, b'Channel 1'), (2, b'Channel 2'), (3, b'Channel 3'), (4, b'Channel 4')]),
            preserve_default=True,
        ),
    ]
