# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0065_device_data_retention_policy'),
    ]

    operations = [
        migrations.AddField(
            model_name='device',
            name='kilowatt_hours_monthly',
            field=models.FloatField(default=0, help_text=b'Monthly killowatt consumption', editable=False),
            preserve_default=True,
        ),
    ]
