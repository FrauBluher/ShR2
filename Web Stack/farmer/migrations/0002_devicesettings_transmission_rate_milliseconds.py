# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('farmer', '0001_initial'),
    ]

    operations = [
        migrations.AddField(
            model_name='devicesettings',
            name='transmission_rate_milliseconds',
            field=models.IntegerField(default=1000),
            preserve_default=True,
        ),
    ]
