# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations
import geoposition.fields


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0019_device_position'),
    ]

    operations = [
        migrations.AlterField(
            model_name='device',
            name='position',
            field=geoposition.fields.GeopositionField(max_length=42, null=True, blank=True),
            preserve_default=True,
        ),
    ]
