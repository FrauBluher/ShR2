# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('debug', '0003_auto_20150402_1606'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='testdatapoint',
            name='appliance',
        ),
        migrations.RemoveField(
            model_name='testevent',
            name='dataPoint',
        ),
        migrations.DeleteModel(
            name='TestDataPoint',
        ),
        migrations.AddField(
            model_name='testevent',
            name='dataPoints',
            field=models.CharField(default=1, max_length=1000),
            preserve_default=False,
        ),
    ]
