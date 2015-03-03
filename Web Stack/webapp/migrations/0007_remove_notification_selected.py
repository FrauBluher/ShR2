# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0006_auto_20150302_2046'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='notification',
            name='selected',
        ),
    ]
