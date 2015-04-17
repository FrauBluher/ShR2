# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0050_auto_20150417_1919'),
    ]

    operations = [
        migrations.CreateModel(
            name='DeviceSettings',
            fields=[
                ('device_serial', models.IntegerField(unique=True, serialize=False, editable=False, primary_key=True)),
                ('main_channel', models.IntegerField(default=1, max_length=1, choices=[(1, b'Channel 1'), (2, b'Channel 2'), (3, b'Channel 3'), (4, b'Channel 4')])),
                ('device', models.OneToOneField(to='microdata.Device')),
            ],
            options={
            },
            bases=(models.Model,),
        ),
    ]
