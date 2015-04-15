# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0041_room_device'),
    ]

    operations = [
        migrations.AddField(
            model_name='roomtype',
            name='appliances',
            field=models.ManyToManyField(to='microdata.Appliance'),
            preserve_default=True,
        ),
    ]
