# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0038_room_device'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='room',
            name='device',
        ),
    ]
