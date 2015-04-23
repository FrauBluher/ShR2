# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0035_remove_intervalnotification_keyword'),
    ]

    operations = [
        migrations.AlterField(
            model_name='usersettings',
            name='event_notification',
            field=models.ManyToManyField(to='webapp.EventNotification', blank=True),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='usersettings',
            name='interval_notification',
            field=models.ManyToManyField(to='webapp.IntervalNotification', blank=True),
            preserve_default=True,
        ),
    ]
