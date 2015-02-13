# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0017_auto_20150213_0500'),
    ]

    operations = [
        migrations.AlterField(
            model_name='device',
            name='registered',
            field=models.BooleanField(default=False, editable=False),
            preserve_default=True,
        ),
    ]
