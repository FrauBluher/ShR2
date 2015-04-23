# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0034_intervalnotification_recurrences'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='intervalnotification',
            name='keyword',
        ),
    ]
