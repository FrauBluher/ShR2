# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0020_auto_20150226_0217'),
    ]

    operations = [
        migrations.AddField(
            model_name='event',
            name='current',
            field=models.FloatField(null=True, blank=True),
            preserve_default=True,
        ),
        migrations.AddField(
            model_name='event',
            name='voltage',
            field=models.FloatField(null=True, blank=True),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='event',
            name='wattage',
            field=models.FloatField(null=True, blank=True),
            preserve_default=True,
        ),
    ]
