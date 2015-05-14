# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0054_auto_20150423_1425'),
    ]

    operations = [
        migrations.AlterField(
            model_name='circuittype',
            name='appliances',
            field=models.ManyToManyField(to='microdata.Appliance', blank=True),
            preserve_default=True,
        ),
    ]
