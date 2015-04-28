# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0055_auto_20150423_1816'),
    ]

    operations = [
        migrations.AlterField(
            model_name='circuit',
            name='circuittype',
            field=models.ForeignKey(to='microdata.CircuitType', null=True),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='circuit',
            name='name',
            field=models.CharField(max_length=50, null=True),
            preserve_default=True,
        ),
    ]
