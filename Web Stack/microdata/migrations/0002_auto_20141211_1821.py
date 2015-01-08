# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations
import jsonfield.fields


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0001_initial'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='event',
            name='timestamp',
        ),
        migrations.RemoveField(
            model_name='event',
            name='wattage',
        ),
        migrations.AlterField(
            model_name='event',
            name='jsonfield',
            field=jsonfield.fields.JSONField(help_text=b'{values:[{milliseconds,float},...]}'),
            preserve_default=True,
        ),
    ]
