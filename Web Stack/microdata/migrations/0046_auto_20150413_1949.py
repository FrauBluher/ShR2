# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0045_auto_20150413_1947'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='devicesettings',
            name='device_serial',
        ),
        migrations.AddField(
            model_name='devicesettings',
            name='id',
            field=models.AutoField(auto_created=True, primary_key=True, default=1, serialize=False, verbose_name='ID'),
            preserve_default=False,
        ),
    ]
