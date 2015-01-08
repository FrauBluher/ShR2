# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0002_auto_20141211_1821'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='event',
            name='jsonfield',
        ),
        migrations.AddField(
            model_name='event',
            name='timestamp',
            field=models.IntegerField(default=1, help_text=b'millisecond resolution', max_length=13),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='event',
            name='wattage',
            field=models.FloatField(default=1),
            preserve_default=False,
        ),
    ]
