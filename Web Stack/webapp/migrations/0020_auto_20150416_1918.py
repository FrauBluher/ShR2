# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0019_auto_20150416_1915'),
    ]

    operations = [
        migrations.RenameField(
            model_name='usersettings',
            old_name='event_notificaiton',
            new_name='event_notification',
        ),
    ]
