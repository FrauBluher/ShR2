# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0004_devicesettings'),
    ]

    operations = [
        migrations.CreateModel(
            name='RatePlan',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('description', models.CharField(max_length=300)),
            ],
            options={
            },
            bases=(models.Model,),
        ),
        migrations.CreateModel(
            name='Territory',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('description', models.CharField(max_length=300)),
            ],
            options={
            },
            bases=(models.Model,),
        ),
        migrations.AddField(
            model_name='devicesettings',
            name='rate_plans',
            field=models.ManyToManyField(to='webapp.RatePlan'),
            preserve_default=True,
        ),
        migrations.AddField(
            model_name='devicesettings',
            name='territories',
            field=models.ManyToManyField(to='webapp.Territory'),
            preserve_default=True,
        ),
    ]
