# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0008_auto_20150303_2000'),
    ]

    operations = [
        migrations.RenameModel(
            old_name='DeviceSettings',
            new_name='DeviceWebSettings',
        ),
        migrations.AlterModelOptions(
            name='territory',
            options={'verbose_name_plural': 'Territories'},
        ),
        migrations.AlterModelOptions(
            name='utilitycompany',
            options={'verbose_name_plural': 'Utility Companies'},
        ),
    ]
