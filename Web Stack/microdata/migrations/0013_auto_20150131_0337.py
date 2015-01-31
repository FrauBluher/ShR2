# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0012_auto_20150131_0239'),
    ]

    operations = [
        migrations.AlterField(
            model_name='event',
            name='device',
            field=models.ForeignKey(to='microdata.Device'),
            preserve_default=True,
        ),
    ]
