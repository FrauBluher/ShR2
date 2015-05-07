# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0045_tier_tier_level'),
    ]

    operations = [
        migrations.AlterField(
            model_name='tier',
            name='max_multiplier_of_baseline',
            field=models.FloatField(null=True, blank=True),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='tier',
            name='rate',
            field=models.FloatField(help_text=b'$', null=True, blank=True),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='tier',
            name='tier_level',
            field=models.IntegerField(null=True, blank=True),
            preserve_default=True,
        ),
    ]
