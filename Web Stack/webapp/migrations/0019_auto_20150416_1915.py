# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0018_notification_event_notificaiton'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='notification',
            name='event_notificaiton',
        ),
        migrations.AddField(
            model_name='usersettings',
            name='event_notificaiton',
            field=models.ManyToManyField(to='webapp.EventNotification'),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='eventnotification',
            name='keyword',
            field=models.CharField(help_text=b'Keyword used to launch manage.py email_event', max_length=300),
            preserve_default=True,
        ),
    ]
