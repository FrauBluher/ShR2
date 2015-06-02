Webapp Application
==================

This application is what interfaces with the data that is retrieved by the Microdata application. Developed as a sort of proof-of-concept, this application has the minimal functionality required to visualize the data coming from the devices in a somewhat meaningful way.

This application is responsible for serving requests from clients on the web by interfacing with the database to display the data. Many of the facets of this application were designed to be interacted with via AJAX, giving the dashboard the responsiveness necessary to provide a useful user experience. Some of the calls to the database can be rather costly in terms of time, so they are lazy loaded, as in only when needed.

In addition, the webapp module houses the HTML and Javascript needed to run the web interface on the client side.

Subpackages
-----------

.. toctree::

    webapp.management

Submodules
----------

webapp.admin module
-------------------

.. automodule:: webapp.admin
    :members:
    :undoc-members:
    :show-inheritance:

webapp.device_dictionary module
-------------------------------

.. automodule:: webapp.device_dictionary
    :members:
    :undoc-members:
    :show-inheritance:

webapp.models module
--------------------

.. automodule:: webapp.models
    :members:
    :undoc-members:
    :show-inheritance:

webapp.tests module
-------------------

.. automodule:: webapp.tests
    :members:
    :undoc-members:
    :show-inheritance:

webapp.timeseries module
------------------------

.. automodule:: webapp.timeseries
    :members:
    :undoc-members:
    :show-inheritance:

webapp.views module
-------------------

.. automodule:: webapp.views
    :members:
    :undoc-members:
    :show-inheritance:


Module contents
---------------

.. automodule:: webapp
    :members:
    :undoc-members:
    :show-inheritance:
