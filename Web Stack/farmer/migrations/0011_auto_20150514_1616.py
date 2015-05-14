# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('farmer', '0010_auto_20150514_1615'),
    ]

    operations = [
        migrations.AlterField(
            model_name='devicesettings',
            name='adc_sample_rate',
            field=models.IntegerField(default=5, max_length=1, choices=[(0, 125000), (1, 62500), (2, 31250), (3, 15625), (4, 7812.5), (5, 3906.25), (6, 1953.125), (7, 976.5625)]),
            preserve_default=True,
        ),
    ]
