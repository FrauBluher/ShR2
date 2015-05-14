# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0054_tier_chart_color'),
    ]

    operations = [
        migrations.AlterField(
            model_name='devicewebsettings',
            name='current_tier',
            field=models.ForeignKey(blank=True, editable=False, to='webapp.Tier', null=True),
            preserve_default=True,
        ),
    ]
