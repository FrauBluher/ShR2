# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations
from django.conf import settings
import jsonfield.fields


class Migration(migrations.Migration):

    dependencies = [
        migrations.swappable_dependency(settings.AUTH_USER_MODEL),
    ]

    operations = [
        migrations.CreateModel(
            name='Appliance',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('serial', models.IntegerField(unique=True)),
                ('name', models.CharField(unique=True, max_length=50)),
            ],
            options={
            },
            bases=(models.Model,),
        ),
        migrations.CreateModel(
            name='Device',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('serial', models.IntegerField(unique=True)),
                ('name', models.CharField(max_length=30)),
                ('zipcode', models.CharField(max_length=5)),
                ('private', models.BooleanField(default=False)),
                ('owner', models.ForeignKey(to=settings.AUTH_USER_MODEL)),
            ],
            options={
            },
            bases=(models.Model,),
        ),
        migrations.CreateModel(
            name='Event',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('event_code', models.IntegerField(null=True, blank=True)),
                ('wattage', models.FloatField()),
                ('timestamp', models.IntegerField()),
                ('jsonfield', jsonfield.fields.JSONField()),
                ('appliance', models.ForeignKey(related_name='appliance', blank=True, to='microdata.Appliance', null=True)),
                ('device', models.ForeignKey(related_name='events', to='microdata.Device')),
            ],
            options={
            },
            bases=(models.Model,),
        ),
    ]
