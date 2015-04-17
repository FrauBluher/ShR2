# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0027_auto_20150416_2039'),
    ]

    operations = [
        migrations.AlterField(
            model_name='intervalnotification',
            name='email_body',
            field=models.FileField(upload_to=b'/home/ubuntu/seads-git/ShR2/Web Stack/static/'),
            preserve_default=True,
        ),
    ]
