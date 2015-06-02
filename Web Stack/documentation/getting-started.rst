.. _getting-started:

Getting Started
============

At this point, the project is now ready to begin accepting clients and devices.

To get devices connected to your project, simply point their database address at the server that faces the internet (for the scalable framework, this is the stateless server or load balancer). This will cause the devices to attempt to find themselves in the server, fail, then register themselves in the system.

From here, users can register to the website and pair to devices via their serial number.

API Documentation
-----------------

You can interact with the server's API by navigating to /docs. This interface was designed to allow developers easy access to the framework so that they can learn it rapidly and integrate new types of devices into the database.

.. image:: http://i.imgur.com/C8xDg4s.png
	:align: center

The REST API is set up to be very trusting. There are no checks for malicious behavior and we assume all users are benign. It is possible to alter any and all properties of a device such as changing the owner via the API. In the future, it is recommended to include API token authentication to prevent malicious attacks.


Administrative Interface
------------------------

In addition to the front facing web interface, there is an administrative interface for managing database models directly. This interface was designed to allow an administrator the ability to alter the way the website functions without having to interface with the source code directly.

An administrator can use this interface to create/modify devices, circuit types, and appliance directly. This would presumably become useful when the algorithms on the SEAD Light mature to the point where disaggregation by appliance is feasable.

In addition, an administrator can also interface with the facets of the web application, including how event notifications are handled as well as add/modify interval notifications. These are the emails sent to users after a certain event has been detected or an interval has elapsed.

There is the ability to add/modify rate plans, territories, and utility companies to the web application, giving more realistic cost predictions for a user's device.
