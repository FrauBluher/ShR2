# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0005_auto_20141211_1920'),
    ]

    operations = [
        migrations.AlterField(
            model_name='event',
            name='timestamp',
            field=models.PositiveIntegerField(help_text=b'13 digits, millisecond resolution'),
            preserve_default=True,
        ),
    ]
