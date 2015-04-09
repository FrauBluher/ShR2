# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0027_auto_20150409_1507'),
    ]

    operations = [
        migrations.AddField(
            model_name='device',
            name='ip_address',
            field=models.GenericIPAddressField(default='192.168.1.1'),
            preserve_default=False,
        ),
    ]
