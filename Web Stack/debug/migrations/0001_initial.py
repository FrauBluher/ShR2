# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0025_auto_20150310_2153'),
    ]

    operations = [
        migrations.CreateModel(
            name='TestDataPoint',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('timestamp', models.PositiveIntegerField(help_text=b'13 digits, millisecond resolution')),
                ('wattage', models.FloatField(null=True, blank=True)),
                ('current', models.FloatField(null=True, blank=True)),
                ('voltage', models.FloatField(null=True, blank=True)),
                ('appliance', models.ForeignKey(blank=True, to='microdata.Appliance', null=True)),
            ],
            options={
            },
            bases=(models.Model,),
        ),
        migrations.CreateModel(
            name='TestEvent',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('dataPoint', models.ManyToManyField(to='debug.TestDataPoint')),
                ('device', models.ForeignKey(to='microdata.Device')),
            ],
            options={
            },
            bases=(models.Model,),
        ),
    ]
