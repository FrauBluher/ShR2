# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('microdata', '0020_auto_20150226_0217'),
        ('webapp', '0003_utilitycompany'),
    ]

    operations = [
        migrations.CreateModel(
            name='DeviceSettings',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('device', models.OneToOneField(to='microdata.Device')),
                ('utility_companies', models.ManyToManyField(to='webapp.UtilityCompany')),
            ],
            options={
            },
            bases=(models.Model,),
        ),
    ]
