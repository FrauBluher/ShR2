# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0040_territory_rate_plan'),
    ]

    operations = [
        migrations.AddField(
            model_name='rateplan',
            name='utility_company',
            field=models.ForeignKey(default=1, to='webapp.UtilityCompany'),
            preserve_default=False,
        ),
    ]
