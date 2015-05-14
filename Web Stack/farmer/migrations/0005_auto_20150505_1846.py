# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations
import datetime
from django.utils.timezone import utc


class Migration(migrations.Migration):

    dependencies = [
        ('farmer', '0004_auto_20150505_1846'),
    ]

    operations = [
        migrations.AlterField(
            model_name='devicesettings',
            name='date_now',
            field=models.DateTimeField(default=datetime.datetime(2015, 5, 5, 18, 46, 21, 610602, tzinfo=utc), auto_now=True),
            preserve_default=True,
        ),
    ]
