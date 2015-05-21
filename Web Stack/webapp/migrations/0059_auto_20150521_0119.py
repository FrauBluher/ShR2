# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0058_auto_20150521_0111'),
    ]

    operations = [
        migrations.AlterField(
            model_name='devicewebsettings',
            name='current_tier',
            field=models.ForeignKey(default=1, to='webapp.Tier'),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='devicewebsettings',
            name='rate_plans',
            field=models.ManyToManyField(default=1, to='webapp.RatePlan'),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='devicewebsettings',
            name='territories',
            field=models.ManyToManyField(default=1, to='webapp.Territory'),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='devicewebsettings',
            name='utility_companies',
            field=models.ManyToManyField(default=1, to='webapp.UtilityCompany'),
            preserve_default=True,
        ),
    ]
