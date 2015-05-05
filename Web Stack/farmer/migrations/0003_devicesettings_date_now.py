# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations
import datetime
from django.utils.timezone import utc


class Migration(migrations.Migration):

    dependencies = [
        ('farmer', '0002_devicesettings_transmission_rate_milliseconds'),
    ]

    operations = [
        migrations.AddField(
            model_name='devicesettings',
            name='date_now',
            field=models.DateTimeField(default=datetime.datetime(2015, 5, 5, 18, 45, 53, 28056, tzinfo=utc), auto_now=True),
            preserve_default=True,
        ),
    ]
