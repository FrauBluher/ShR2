# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0053_auto_20150422_2134'),
    ]

    operations = [
        migrations.RenameField(
            model_name='circuit',
            old_name='roomtype',
            new_name='circuittype',
        ),
    ]
