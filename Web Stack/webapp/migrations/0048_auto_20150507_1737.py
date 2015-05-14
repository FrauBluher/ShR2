# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0047_auto_20150507_1717'),
    ]

    operations = [
        migrations.AlterField(
            model_name='tier',
            name='max_percentage_of_baseline',
            field=models.FloatField(help_text=b'blank for no maximum', null=True, blank=True),
            preserve_default=True,
        ),
    ]
