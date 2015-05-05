# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('farmer', '0006_auto_20150505_1854'),
    ]

    operations = [
        migrations.AlterField(
            model_name='devicesettings',
            name='date_now',
            field=models.IntegerField(null=True, blank=True),
            preserve_default=True,
        ),
    ]
