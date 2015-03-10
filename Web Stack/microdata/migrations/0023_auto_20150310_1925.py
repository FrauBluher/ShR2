# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0022_auto_20150310_1627'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='datapoint',
            name='appliance',
        ),
        migrations.RemoveField(
            model_name='datapoint',
            name='event',
        ),
        migrations.DeleteModel(
            name='DataPoint',
        ),
        migrations.AddField(
            model_name='event',
            name='appliance',
            field=models.ForeignKey(related_name='appliance', blank=True, to='microdata.Appliance', null=True),
            preserve_default=True,
        ),
        migrations.AddField(
            model_name='event',
            name='current',
            field=models.FloatField(null=True, blank=True),
            preserve_default=True,
        ),
        migrations.AddField(
            model_name='event',
            name='event_code',
            field=models.IntegerField(null=True, blank=True),
            preserve_default=True,
        ),
        migrations.AddField(
            model_name='event',
            name='timestamp',
            field=models.PositiveIntegerField(default=1, help_text=b'13 digits, millisecond resolution'),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='event',
            name='voltage',
            field=models.FloatField(null=True, blank=True),
            preserve_default=True,
        ),
        migrations.AddField(
            model_name='event',
            name='wattage',
            field=models.FloatField(null=True, blank=True),
            preserve_default=True,
        ),
    ]
