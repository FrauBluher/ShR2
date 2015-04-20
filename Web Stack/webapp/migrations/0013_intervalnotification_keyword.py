# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0012_auto_20150415_1928'),
    ]

    operations = [
        migrations.AddField(
            model_name='intervalnotification',
            name='keyword',
            field=models.CharField(default=1, help_text=b'Keyword used to launch manage.py email_notification', max_length=300),
            preserve_default=False,
        ),
    ]
