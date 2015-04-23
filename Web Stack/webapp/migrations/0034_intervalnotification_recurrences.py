# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations
import recurrence.fields


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0033_auto_20150420_1724'),
    ]

    operations = [
        migrations.AddField(
            model_name='intervalnotification',
            name='recurrences',
            field=recurrence.fields.RecurrenceField(null=True, blank=True),
            preserve_default=True,
        ),
    ]
