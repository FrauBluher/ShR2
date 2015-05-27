# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('farmer', '0011_auto_20150514_1616'),
    ]

    operations = [
        migrations.AlterField(
            model_name='devicesettings',
            name='adc_sample_rate',
            field=models.IntegerField(default=5, choices=[(0, 125000), (1, 62500), (2, 31250), (3, 15625), (4, 7812.5), (5, 3906.25), (6, 1953.125), (7, 976.5625)]),
        ),
        migrations.AlterField(
            model_name='devicesettings',
            name='main_channel',
            field=models.IntegerField(default=1, choices=[(1, b'Channel 1'), (2, b'Channel 2'), (3, b'Channel 3'), (4, b'Channel 4')]),
        ),
    ]
