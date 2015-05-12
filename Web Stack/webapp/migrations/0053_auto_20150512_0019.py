# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0052_auto_20150511_2324'),
    ]

    operations = [
        migrations.AlterField(
            model_name='territory',
            name='summer_start',
            field=models.IntegerField(help_text=b'Specify Month of year', null=True, blank=True),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='territory',
            name='winter_start',
            field=models.IntegerField(help_text=b'Specify Month of year', null=True, blank=True),
            preserve_default=True,
        ),
    ]
