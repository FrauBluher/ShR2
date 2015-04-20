# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0039_remove_room_device'),
    ]

    operations = [
        migrations.AddField(
            model_name='room',
            name='roomtype',
            field=models.ForeignKey(default=1, to='microdata.RoomType'),
            preserve_default=False,
        ),
    ]
