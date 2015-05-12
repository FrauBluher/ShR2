# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0048_auto_20150507_1737'),
    ]

    operations = [
        migrations.AddField(
            model_name='devicewebsettings',
            name='current_tier',
            field=models.ForeignKey(default=1, to='webapp.Tier'),
            preserve_default=False,
        ),
    ]
