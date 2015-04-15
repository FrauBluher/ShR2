# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0033_auto_20150409_1819'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='room',
            name='device',
        ),
        migrations.RemoveField(
            model_name='room',
            name='roomtype',
        ),
        migrations.DeleteModel(
            name='Room',
        ),
        migrations.DeleteModel(
            name='RoomType',
        ),
    ]
