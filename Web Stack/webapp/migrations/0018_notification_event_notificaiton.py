# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0017_auto_20150416_1910'),
    ]

    operations = [
        migrations.AddField(
            model_name='notification',
            name='event_notificaiton',
            field=models.ManyToManyField(to='webapp.EventNotification'),
            preserve_default=True,
        ),
    ]
