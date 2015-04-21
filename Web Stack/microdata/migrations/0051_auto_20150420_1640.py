# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0050_auto_20150417_1919'),
    ]

    operations = [
        migrations.CreateModel(
            name='Circuit',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('name', models.CharField(max_length=50)),
                ('device', models.ForeignKey(to='microdata.Device')),
            ],
            options={
            },
            bases=(models.Model,),
        ),
        migrations.RenameModel(
            old_name='RoomType',
            new_name='CircuitType',
        ),
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
        migrations.AddField(
            model_name='circuit',
            name='roomtype',
            field=models.ForeignKey(to='microdata.CircuitType'),
            preserve_default=True,
        ),
    ]
