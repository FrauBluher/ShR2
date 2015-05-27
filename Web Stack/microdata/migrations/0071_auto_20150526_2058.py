# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations
from django.conf import settings


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0070_circuittype_chart_color'),
    ]

    operations = [
        migrations.AddField(
            model_name='device',
            name='cost_daily',
            field=models.FloatField(default=0, help_text=b'Daily cost', editable=False),
        ),
        migrations.AlterField(
            model_name='device',
            name='channel_1',
            field=models.ForeignKey(related_name='Channel 1+', blank=True, to='microdata.CircuitType', null=True),
        ),
        migrations.AlterField(
            model_name='device',
            name='channel_2',
            field=models.ForeignKey(related_name='Channel 2+', blank=True, to='microdata.CircuitType', null=True),
        ),
        migrations.AlterField(
            model_name='device',
            name='channel_3',
            field=models.ForeignKey(related_name='Channel 3+', blank=True, to='microdata.CircuitType', null=True),
        ),
        migrations.AlterField(
            model_name='device',
            name='share_with',
            field=models.ManyToManyField(related_name='share_with', to=settings.AUTH_USER_MODEL, blank=True),
        ),
    ]
