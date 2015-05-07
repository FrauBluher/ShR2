# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0046_auto_20150507_1717'),
    ]

    operations = [
        migrations.RenameField(
            model_name='tier',
            old_name='max_multiplier_of_baseline',
            new_name='max_percentage_of_baseline',
        ),
    ]
