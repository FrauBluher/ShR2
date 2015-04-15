# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0043_devicesettings'),
    ]

    operations = [
        migrations.AlterField(
            model_name='devicesettings',
            name='device',
            field=models.OneToOneField(to='microdata.Device'),
            preserve_default=True,
        ),
    ]
