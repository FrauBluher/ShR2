# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0038_auto_20150507_1628'),
    ]

    operations = [
        migrations.RenameField(
            model_name='territory',
            old_name='source',
            new_name='data_source',
        ),
        migrations.AddField(
            model_name='rateplan',
            name='baseline_usage',
            field=models.FloatField(default=1),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='rateplan',
            name='between_101_and_130',
            field=models.FloatField(default=1),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='rateplan',
            name='between_131_and_200',
            field=models.FloatField(default=1),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='rateplan',
            name='between_201_and_300',
            field=models.FloatField(default=1),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='rateplan',
            name='california_climate_credit',
            field=models.FloatField(default=1),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='rateplan',
            name='data_source',
            field=models.URLField(default=1),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='rateplan',
            name='min_charge_rate',
            field=models.FloatField(default=1),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='rateplan',
            name='over_300',
            field=models.FloatField(default=1),
            preserve_default=False,
        ),
    ]
