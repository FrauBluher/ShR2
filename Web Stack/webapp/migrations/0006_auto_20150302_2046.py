# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0005_auto_20150226_0428'),
    ]

    operations = [
        migrations.AddField(
            model_name='notification',
            name='selected',
            field=models.BooleanField(default=False),
            preserve_default=True,
        ),
        migrations.AddField(
            model_name='rateplan',
            name='selected',
            field=models.BooleanField(default=False),
            preserve_default=True,
        ),
        migrations.AddField(
            model_name='territory',
            name='selected',
            field=models.BooleanField(default=False),
            preserve_default=True,
        ),
        migrations.AddField(
            model_name='utilitycompany',
            name='selected',
            field=models.BooleanField(default=False),
            preserve_default=True,
        ),
    ]
