# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0021_auto_20150305_2229'),
    ]

    operations = [
        migrations.CreateModel(
            name='DataPoint',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('event_code', models.IntegerField(null=True, blank=True)),
                ('timestamp', models.PositiveIntegerField(help_text=b'13 digits, millisecond resolution')),
                ('wattage', models.FloatField(null=True, blank=True)),
                ('current', models.FloatField(null=True, blank=True)),
                ('voltage', models.FloatField(null=True, blank=True)),
                ('appliance', models.ForeignKey(related_name='appliance', blank=True, to='microdata.Appliance', null=True)),
                ('event', models.ForeignKey(to='microdata.Event')),
            ],
            options={
            },
            bases=(models.Model,),
        ),
        migrations.RemoveField(
            model_name='event',
            name='appliance',
        ),
        migrations.RemoveField(
            model_name='event',
            name='current',
        ),
        migrations.RemoveField(
            model_name='event',
            name='event_code',
        ),
        migrations.RemoveField(
            model_name='event',
            name='timestamp',
        ),
        migrations.RemoveField(
            model_name='event',
            name='voltage',
        ),
        migrations.RemoveField(
            model_name='event',
            name='wattage',
        ),
    ]
