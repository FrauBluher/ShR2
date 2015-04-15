# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0037_room_roomtype'),
    ]

    operations = [
        migrations.AddField(
            model_name='room',
            name='device',
            field=models.ForeignKey(default=1, to='microdata.Device'),
            preserve_default=False,
        ),
    ]
