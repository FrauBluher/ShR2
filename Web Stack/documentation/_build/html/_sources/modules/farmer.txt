Farmer Application
==================

This barebones application was put in place to manage devices via the
REST API.

As it stands now, this application is responsible for maintaining the
settings of each device and serving the configurations via API calls.

A typical API call to get the settings of a device is as follows::

    GET /api/settings-api/{serial}/
    
    RESPONSE

        {
            "device": 3,
            "device_serial": 3,
            "main_channel": 1,
            "transmission_rate_milliseconds": 5000,
            "adc_sample_rate": 7,
            "date_now": 1433354266811
        }

The SEAD Light takes advantage of some, but not all, of these fields:

* transmission_rate_milliseconds: The time between each packet transmit

* adc_sample_rate: A binary-mapped value `of choices <farmer.html#farmer.models.DeviceSettings.SAMPLE_RATE_CHOICES>`_.

* date_now: This is used by the SEAD Light to synchronize its RTC (Real Time Clock)

Submodules
----------

farmer.admin module
-------------------

.. automodule:: farmer.admin
    :members:
    :undoc-members:
    :show-inheritance:

farmer.models module
--------------------

.. automodule:: farmer.models
    :members:
    :undoc-members:
    :show-inheritance:

farmer.serializers module
-------------------------

.. automodule:: farmer.serializers
    :members:
    :undoc-members:
    :show-inheritance:

farmer.tests module
-------------------

.. automodule:: farmer.tests
    :members:
    :undoc-members:
    :show-inheritance:

farmer.views module
-------------------

.. automodule:: farmer.views
    :members:
    :undoc-members:
    :show-inheritance:


Module contents
---------------

.. automodule:: farmer
    :members:
    :undoc-members:
    :show-inheritance:
