# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0042_roomtype_appliances'),
    ]

    operations = [
        migrations.CreateModel(
            name='DeviceSettings',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('main_channel', models.CharField(default=1, max_length=1, choices=[(b'1', b'Channel 1'), (b'2', b'Channel 2'), (b'3', b'Channel 3'), (b'4', b'Channel 4')])),
                ('device', models.ForeignKey(to='microdata.Device')),
            ],
            options={
            },
            bases=(models.Model,),
        ),
    ]
