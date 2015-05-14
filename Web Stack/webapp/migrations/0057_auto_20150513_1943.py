# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0056_auto_20150513_1940'),
    ]

    operations = [
        migrations.AlterField(
            model_name='devicewebsettings',
            name='current_tier',
            field=models.ForeignKey(to='webapp.Tier'),
            preserve_default=True,
        ),
    ]
