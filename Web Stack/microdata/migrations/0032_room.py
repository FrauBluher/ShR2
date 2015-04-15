# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0031_auto_20150409_1812'),
    ]

    operations = [
        migrations.CreateModel(
            name='Room',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('name', models.CharField(default=None, max_length=50, blank=True)),
                ('roomtype', models.ForeignKey(to='microdata.RoomType')),
            ],
            options={
            },
            bases=(models.Model,),
        ),
    ]
