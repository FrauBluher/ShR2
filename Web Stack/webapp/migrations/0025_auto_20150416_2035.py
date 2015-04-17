# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0024_auto_20150416_2034'),
    ]

    operations = [
        migrations.AlterField(
            model_name='intervalnotification',
            name='email_body',
            field=models.FileField(upload_to=b'email'),
            preserve_default=True,
        ),
    ]
