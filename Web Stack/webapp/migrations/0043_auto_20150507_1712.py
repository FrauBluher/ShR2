# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0042_auto_20150507_1712'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='rateplan',
            name='tier_1',
        ),
        migrations.RemoveField(
            model_name='rateplan',
            name='tier_2',
        ),
        migrations.RemoveField(
            model_name='rateplan',
            name='tier_3',
        ),
        migrations.RemoveField(
            model_name='rateplan',
            name='tier_4',
        ),
        migrations.RemoveField(
            model_name='rateplan',
            name='tier_5',
        ),
    ]
