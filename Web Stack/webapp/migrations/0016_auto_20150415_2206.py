# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations
from django.conf import settings


class Migration(migrations.Migration):

    dependencies = [
        migrations.swappable_dependency(settings.AUTH_USER_MODEL),
        ('webapp', '0015_remove_intervalnotification_selected'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='usersettings',
            name='notifications',
        ),
        migrations.AddField(
            model_name='notification',
            name='user',
            field=models.OneToOneField(default=1, to=settings.AUTH_USER_MODEL),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='usersettings',
            name='interval_notification',
            field=models.ManyToManyField(to='webapp.IntervalNotification'),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='intervalnotification',
            name='keyword',
            field=models.CharField(help_text=b'Keyword used to launch manage.py email_interval', max_length=300),
            preserve_default=True,
        ),
    ]
