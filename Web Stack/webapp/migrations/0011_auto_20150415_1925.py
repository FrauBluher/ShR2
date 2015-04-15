# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0010_dashboardsettings'),
    ]

    operations = [
        migrations.CreateModel(
            name='IntervalNotification',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('interval', models.CharField(max_length=300)),
                ('interval_adverb', models.CharField(max_length=300)),
                ('time_delta', models.FloatField(help_text=b'Time in seconds between intervals')),
            ],
            options={
            },
            bases=(models.Model,),
        ),
        migrations.AddField(
            model_name='notification',
            name='interval_notification',
            field=models.ManyToManyField(to='webapp.IntervalNotification'),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='dashboardsettings',
            name='stack',
            field=models.BooleanField(default=True, help_text=b'Specifies the default behavior for a graph: stacked or unstacked line chart'),
            preserve_default=True,
        ),
    ]
