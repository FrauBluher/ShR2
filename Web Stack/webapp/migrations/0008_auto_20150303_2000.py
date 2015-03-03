# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0007_remove_notification_selected'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='rateplan',
            name='selected',
        ),
        migrations.RemoveField(
            model_name='territory',
            name='selected',
        ),
        migrations.RemoveField(
            model_name='utilitycompany',
            name='selected',
        ),
    ]
