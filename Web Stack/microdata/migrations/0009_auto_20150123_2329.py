# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0008_auto_20150123_2054'),
    ]

    operations = [
        migrations.AlterField(
            model_name='device',
            name='name',
            field=models.CharField(default='a', max_length=30),
            preserve_default=False,
        ),
    ]
