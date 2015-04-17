# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0021_eventnotification_email_subject'),
    ]

    operations = [
        migrations.AddField(
            model_name='eventnotification',
            name='email_body',
            field=models.FileField(default=1, upload_to=b''),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='intervalnotification',
            name='email_body',
            field=models.FileField(default=1, upload_to=b''),
            preserve_default=False,
        ),
    ]
