# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0014_auto_20150131_0345'),
    ]

    operations = [
        migrations.AddField(
            model_name='device',
            name='continuous_queries_registered',
            field=models.BooleanField(default=False, editable=False),
            preserve_default=True,
        ),
    ]
