# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0037_auto_20150507_1627'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='utilitycompany',
            name='source',
        ),
        migrations.RemoveField(
            model_name='utilitycompany',
            name='summer_rate',
        ),
        migrations.RemoveField(
            model_name='utilitycompany',
            name='winter_rate',
        ),
        migrations.AddField(
            model_name='territory',
            name='source',
            field=models.URLField(default=1),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='territory',
            name='summer_rate',
            field=models.FloatField(default=1),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='territory',
            name='winter_rate',
            field=models.FloatField(default=1),
            preserve_default=False,
        ),
    ]
