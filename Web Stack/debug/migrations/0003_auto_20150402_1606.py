# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('debug', '0002_auto_20150402_1604'),
    ]

    operations = [
        migrations.AlterField(
            model_name='testevent',
            name='dataPoint',
            field=models.ManyToManyField(to='debug.TestDataPoint'),
            preserve_default=True,
        ),
    ]
