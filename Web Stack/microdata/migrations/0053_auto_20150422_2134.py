# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0052_auto_20150422_2127'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='device',
            name='circuits',
        ),
        migrations.AddField(
            model_name='circuit',
            name='device',
            field=models.ForeignKey(default=1, to='microdata.Device'),
            preserve_default=False,
        ),
    ]
