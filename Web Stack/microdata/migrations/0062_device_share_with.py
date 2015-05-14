# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations
from django.conf import settings


class Migration(migrations.Migration):

    dependencies = [
        migrations.swappable_dependency(settings.AUTH_USER_MODEL),
        ('microdata', '0061_auto_20150507_1656'),
    ]

    operations = [
        migrations.AddField(
            model_name='device',
            name='share_with',
            field=models.ManyToManyField(related_name='share_with', null=True, to=settings.AUTH_USER_MODEL, blank=True),
            preserve_default=True,
        ),
    ]
