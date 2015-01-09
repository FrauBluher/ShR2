# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0004_auto_20141211_1916'),
    ]

    operations = [
        migrations.AlterField(
            model_name='event',
            name='timestamp',
            field=models.PositiveIntegerField(help_text=b'13 digits, millisecond resolution', max_length=13),
            preserve_default=True,
        ),
    ]
