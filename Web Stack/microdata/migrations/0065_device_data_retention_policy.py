# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0064_event_query'),
    ]

    operations = [
        migrations.AddField(
            model_name='device',
            name='data_retention_policy',
            field=models.IntegerField(default=12, help_text=b'Number of months of data to keep in database'),
            preserve_default=True,
        ),
    ]
