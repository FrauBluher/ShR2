# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations
import paintstore.fields


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0053_auto_20150512_0019'),
    ]

    operations = [
        migrations.AddField(
            model_name='tier',
            name='chart_color',
            field=paintstore.fields.ColorPickerField(default=1, max_length=7),
            preserve_default=False,
        ),
    ]
