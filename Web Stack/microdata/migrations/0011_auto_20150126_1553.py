# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0010_auto_20150123_2337'),
    ]

    operations = [
        migrations.AlterField(
            model_name='device',
            name='secret_key',
            field=models.CharField(max_length=7, null=True, editable=False, blank=True),
            preserve_default=True,
        ),
    ]
