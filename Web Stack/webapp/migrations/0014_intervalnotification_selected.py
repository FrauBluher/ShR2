# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0013_intervalnotification_keyword'),
    ]

    operations = [
        migrations.AddField(
            model_name='intervalnotification',
            name='selected',
            field=models.BooleanField(default=False),
            preserve_default=True,
        ),
    ]
