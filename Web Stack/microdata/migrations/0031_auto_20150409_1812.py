# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0030_room'),
    ]

    operations = [
        migrations.RenameModel(
            old_name='Room',
            new_name='RoomType',
        ),
    ]
