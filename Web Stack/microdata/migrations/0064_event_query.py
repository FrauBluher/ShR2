# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0063_auto_20150510_0249'),
    ]

    operations = [
        migrations.AddField(
            model_name='event',
            name='query',
            field=models.CharField(default=1, max_length=1000),
            preserve_default=False,
        ),
    ]
