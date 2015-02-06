# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0015_device_continuous_queries_registered'),
    ]

    operations = [
        migrations.RenameField(
            model_name='device',
            old_name='continuous_queries_registered',
            new_name='fanout_query_registered',
        ),
    ]
