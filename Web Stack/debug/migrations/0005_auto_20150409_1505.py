# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('debug', '0004_auto_20150402_1921'),
    ]

    operations = [
        migrations.AlterField(
            model_name='testevent',
            name='dataPoints',
            field=models.CharField(help_text=b'Expects a JSON encoded string of values: [{timestamp(int), wattage(float, optional)},...]', max_length=1000),
            preserve_default=True,
        ),
    ]
