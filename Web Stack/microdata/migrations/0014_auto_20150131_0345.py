# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0013_auto_20150131_0337'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='device',
            name='id',
        ),
        migrations.AlterField(
            model_name='device',
            name='serial',
            field=models.IntegerField(unique=True, serialize=False, primary_key=True),
            preserve_default=True,
        ),
    ]
