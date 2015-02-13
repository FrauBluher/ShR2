# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0016_auto_20150205_2006'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='device',
            name='private',
        ),
        migrations.RemoveField(
            model_name='device',
            name='zipcode',
        ),
    ]
