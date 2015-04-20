# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0049_auto_20150415_1645'),
        ('webapp', '0016_auto_20150415_2206'),
    ]

    operations = [
        migrations.CreateModel(
            name='EventNotification',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('description', models.CharField(help_text=b'Label to notification as shown to a user', max_length=300)),
                ('keyword', models.CharField(help_text=b'Keyword used to launch manage.py email_interval', max_length=300)),
                ('watts_above_average', models.FloatField()),
                ('period_of_time', models.FloatField(help_text=b'Period of time to watch for irregularity')),
                ('appliances_to_watch', models.ManyToManyField(to='microdata.Appliance')),
            ],
            options={
            },
            bases=(models.Model,),
        ),
        migrations.AddField(
            model_name='intervalnotification',
            name='email_subject',
            field=models.CharField(default=1, max_length=300),
            preserve_default=False,
        ),
        migrations.AlterField(
            model_name='intervalnotification',
            name='description',
            field=models.CharField(help_text=b'Label to notification as shown to a user', max_length=300),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='intervalnotification',
            name='interval',
            field=models.CharField(help_text=b'Word descriptor for type of interval (i.e. Day). First letter capitalized.', max_length=300),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='intervalnotification',
            name='interval_adverb',
            field=models.CharField(help_text=b'Adverb of interval (i.e. Daily). First letter capitalized.', max_length=300),
            preserve_default=True,
        ),
    ]
