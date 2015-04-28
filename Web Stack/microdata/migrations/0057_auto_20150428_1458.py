# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0056_auto_20150428_1420'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='circuit',
            name='device',
        ),
        migrations.AddField(
            model_name='device',
            name='channel_0',
            field=models.ForeignKey(related_name='Channel 0', default=1, to='microdata.Circuit'),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='device',
            name='channel_1',
            field=models.ForeignKey(related_name='Channel 1', default=1, to='microdata.Circuit'),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='device',
            name='channel_2',
            field=models.ForeignKey(related_name='Channel 2', default=1, to='microdata.Circuit'),
            preserve_default=False,
        ),
    ]
