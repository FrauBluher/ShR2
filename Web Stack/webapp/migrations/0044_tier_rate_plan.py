# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0043_auto_20150507_1712'),
    ]

    operations = [
        migrations.AddField(
            model_name='tier',
            name='rate_plan',
            field=models.ForeignKey(default=1, to='webapp.RatePlan'),
            preserve_default=False,
        ),
    ]
