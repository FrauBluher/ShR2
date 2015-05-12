# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0050_auto_20150511_2321'),
    ]

    operations = [
        migrations.AddField(
            model_name='territory',
            name='summer_start',
            field=models.DateField(null=True, blank=True),
            preserve_default=True,
        ),
        migrations.AddField(
            model_name='territory',
            name='winter_start',
            field=models.DateField(null=True, blank=True),
            preserve_default=True,
        ),
    ]
