# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0001_initial'),
    ]

    operations = [
        migrations.RenameModel(
            old_name='Choices',
            new_name='Notification',
        ),
        migrations.RenameField(
            model_name='usersettings',
            old_name='choices',
            new_name='notifications',
        ),
    ]
