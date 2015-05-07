# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0044_tier_rate_plan'),
    ]

    operations = [
        migrations.AddField(
            model_name='tier',
            name='tier_level',
            field=models.IntegerField(default=1),
            preserve_default=False,
        ),
    ]
