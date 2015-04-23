# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0032_auto_20150417_2013'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='intervalnotification',
            name='interval',
        ),
        migrations.RemoveField(
            model_name='intervalnotification',
            name='interval_adverb',
        ),
        migrations.RemoveField(
            model_name='intervalnotification',
            name='time_delta',
        ),
        migrations.AlterField(
            model_name='eventnotification',
            name='email_body',
            field=models.FileField(help_text=b'Template file for email body. {{ x }} denotes template variable', upload_to=b''),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='intervalnotification',
            name='email_body',
            field=models.FileField(help_text=b'Template file for email body. {{ x }} denotes template variable', upload_to=b''),
            preserve_default=True,
        ),
    ]
