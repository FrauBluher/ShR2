Microdata Application
=====================

This application is the direct interface between devices and the InfluxDB database. It is responsible for exposing the REST API endpoints.

A :class:`microdata.models.Device` is what links to a SEAD Light out in the world. When a new SEAD connects to the system, it will check to see
if it has been registered on the system by querying `/api/device-api/{serial}/`. If the response is 404, the device will then register
on the system as new with no owner. It is then the user's responsibility to pair the device via the web application in order to access
the data.

Refer to the :ref:`getting-started` guide for more information on interfacing the devices with the server.


Subpackages
-----------

.. toctree::

    microdata.management

Submodules
----------

microdata.admin module
----------------------

Models registered to the administrative interface are listed below. These are the interfaces provided to an administrator that may have control over the system for a group of users.

.. automodule:: microdata.admin
    :members:
    :undoc-members:
    :show-inheritance:

microdata.models module
-----------------------

.. automodule:: microdata.models
    :members:
    :undoc-members:
    :show-inheritance:

microdata.serializers module
----------------------------

.. automodule:: microdata.serializers
    :members:
    :undoc-members:
    :show-inheritance:

microdata.tests module
----------------------

.. automodule:: microdata.tests
    :members:
    :undoc-members:
    :show-inheritance:

microdata.views module
----------------------

.. automodule:: microdata.views
    :members:
    :undoc-members:
    :show-inheritance:


Module contents
---------------

.. automodule:: microdata
    :members:
    :undoc-members:
    :show-inheritance:
