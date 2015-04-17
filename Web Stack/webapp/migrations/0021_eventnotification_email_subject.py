# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0020_auto_20150416_1918'),
    ]

    operations = [
        migrations.AddField(
            model_name='eventnotification',
            name='email_subject',
            field=models.CharField(default=1, max_length=300),
            preserve_default=False,
        ),
    ]
