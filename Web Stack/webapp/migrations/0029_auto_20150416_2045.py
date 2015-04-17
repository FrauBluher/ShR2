# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0028_auto_20150416_2041'),
    ]

    operations = [
        migrations.AlterField(
            model_name='intervalnotification',
            name='email_body',
            field=models.FileField(upload_to=b''),
            preserve_default=True,
        ),
    ]
