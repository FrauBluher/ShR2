# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0036_auto_20150421_0021'),
    ]

    operations = [
        migrations.AddField(
            model_name='utilitycompany',
            name='source',
            field=models.URLField(default=1),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='utilitycompany',
            name='summer_rate',
            field=models.FloatField(default=1),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='utilitycompany',
            name='winter_rate',
            field=models.FloatField(default=1),
            preserve_default=False,
        ),
    ]
