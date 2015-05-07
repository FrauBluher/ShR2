# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('webapp', '0041_rateplan_utility_company'),
    ]

    operations = [
        migrations.CreateModel(
            name='Tier',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('max_multiplier_of_baseline', models.FloatField()),
                ('rate', models.FloatField(help_text=b'$')),
            ],
            options={
            },
            bases=(models.Model,),
        ),
        migrations.RemoveField(
            model_name='rateplan',
            name='baseline_usage',
        ),
        migrations.RemoveField(
            model_name='rateplan',
            name='between_101_and_130',
        ),
        migrations.RemoveField(
            model_name='rateplan',
            name='between_131_and_200',
        ),
        migrations.RemoveField(
            model_name='rateplan',
            name='between_201_and_300',
        ),
        migrations.RemoveField(
            model_name='rateplan',
            name='over_300',
        ),
        migrations.AddField(
            model_name='rateplan',
            name='tier_1',
            field=models.OneToOneField(related_name='tier_1', default=1, to='webapp.Tier'),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='rateplan',
            name='tier_2',
            field=models.OneToOneField(related_name='tier_2', default=1, to='webapp.Tier'),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='rateplan',
            name='tier_3',
            field=models.OneToOneField(related_name='tier_3', default=1, to='webapp.Tier'),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='rateplan',
            name='tier_4',
            field=models.OneToOneField(related_name='tier_4', default=1, to='webapp.Tier'),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='rateplan',
            name='tier_5',
            field=models.OneToOneField(related_name='tier_5', default=1, to='webapp.Tier'),
            preserve_default=False,
        ),
        migrations.AlterField(
            model_name='rateplan',
            name='california_climate_credit',
            field=models.FloatField(help_text=b'$ Per household, per semi-annual payment occurring in the April and October bill cycles'),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='rateplan',
            name='min_charge_rate',
            field=models.FloatField(help_text=b'$ Per meter per day'),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='territory',
            name='summer_rate',
            field=models.FloatField(help_text=b'Baseline quantity (kWh per day)'),
            preserve_default=True,
        ),
        migrations.AlterField(
            model_name='territory',
            name='winter_rate',
            field=models.FloatField(help_text=b'Baseline quantity (kWh per day)'),
            preserve_default=True,
        ),
    ]
