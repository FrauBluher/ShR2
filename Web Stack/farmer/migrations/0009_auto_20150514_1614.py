# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('farmer', '0008_devicesettings_adc_sample_rate'),
    ]

    operations = [
        migrations.AlterField(
            model_name='devicesettings',
            name='adc_sample_rate',
            field=models.IntegerField(default=6, max_length=1, choices=[(1, 125000), (2, 62500), (3, 31250), (4, 15625), (5, 7812.5), (6, 3906.25), (7, 1953.125), (8, 976.5625)]),
            preserve_default=True,
        ),
    ]
