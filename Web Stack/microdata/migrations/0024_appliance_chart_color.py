# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0023_auto_20150310_1925'),
    ]

    operations = [
        migrations.AddField(
            model_name='appliance',
            name='chart_color',
            field=models.CharField(default=1, max_length=6),
            preserve_default=False,
        ),
    ]
