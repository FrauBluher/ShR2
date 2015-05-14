# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0066_device_kilowatt_hours_monthly'),
    ]

    operations = [
        migrations.AddField(
            model_name='device',
            name='kilowatt_hours_daily',
            field=models.FloatField(default=0, help_text=b'Daily killowatt consumption', editable=False),
            preserve_default=True,
        ),
    ]
