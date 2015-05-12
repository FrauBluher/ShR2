# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0049_devicewebsettings_current_tier'),
    ]

    operations = [
        migrations.AlterField(
            model_name='devicewebsettings',
            name='current_tier',
            field=models.ForeignKey(editable=False, to='webapp.Tier'),
            preserve_default=True,
        ),
    ]
