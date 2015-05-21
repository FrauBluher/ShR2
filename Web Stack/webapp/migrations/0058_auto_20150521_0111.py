# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0057_auto_20150513_1943'),
    ]

    operations = [
        migrations.AlterField(
            model_name='devicewebsettings',
            name='current_tier',
            field=models.ForeignKey(blank=True, to='webapp.Tier', null=True),
            preserve_default=True,
        ),
    ]
