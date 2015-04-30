# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0058_auto_20150428_1516'),
    ]

    operations = [
        migrations.AlterField(
            model_name='device',
            name='channel_0',
            field=models.ForeignKey(related_name='Channel 0', blank=True, to='microdata.CircuitType', null=True),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='device',
            name='channel_1',
            field=models.ForeignKey(related_name='Channel 1', blank=True, to='microdata.CircuitType', null=True),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='device',
            name='channel_2',
            field=models.ForeignKey(related_name='Channel 2', blank=True, to='microdata.CircuitType', null=True),
            preserve_default=True,
        ),
    ]
