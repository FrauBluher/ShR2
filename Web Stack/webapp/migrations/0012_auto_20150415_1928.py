# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0011_auto_20150415_1925'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='notification',
            name='description',
        ),
        migrations.AddField(
            model_name='intervalnotification',
            name='description',
            field=models.CharField(default=1, max_length=300),
            preserve_default=False,
        ),
    ]
