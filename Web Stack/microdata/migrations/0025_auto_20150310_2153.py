# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations
import paintstore.fields


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0024_appliance_chart_color'),
    ]

    operations = [
        migrations.AlterField(
            model_name='appliance',
            name='chart_color',
            field=paintstore.fields.ColorPickerField(max_length=7),
            preserve_default=True,
        ),
    ]
