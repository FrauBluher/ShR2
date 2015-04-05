# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('debug', '0001_initial'),
    ]

    operations = [
        migrations.AlterField(
            model_name='testevent',
            name='dataPoint',
            field=models.ManyToManyField(to='debug.TestDataPoint', null=True, blank=True),
            preserve_default=True,
        ),
    ]
