.. _getting-started:

Getting Started
============

.. Note::
	
	The SEAD Light will henceforth be referred to as a "Device" for sake of generalizing. This framework was built with the SEAD Light in mind, however any type of "device" is compatible if they follow the `API Guidelines <http://seads.io/docs>`_.

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


Connecting Devices to the Framework
-----------------------------------

With the new framework in place, we are now ready to begin connecting devices to the database. The general outline on how this is done is as follows:

1. Point device at the API Endpoint for your framework.

The endpoint of your system depends on whether or not the scalable framework is in place. If it is, then the endpoint is the address of the Load Balancer. If this is an atomic installation, then the endpoint is the address of the machine running the applications.

For the API already in place, this would be::

	http://seads.io/api/

2. Query the devices database to check if the serial of the device is registered::

	GET http://seads.io/api/device-api/{serial}/

If the response is 404 (not found), continue to the next step. If the response is 302 (found), then the device is already connected and ready to transmit.

3. Register the device with the framework::

	POST http://seads.io/api/device-api/?serial={serial}

This POST request will register the device within the server, allocating database series and instantiating web application models that will relate to this device.

At this point, the device is ready to start transmitting packets. The device can transmit packets to the server without an owner. It is assumed that the owner is interested in seeing all data even before the device is paired, so the data is stored regardless of if the device is an orphan or not.

4. Begin data transmission::

	POST http://seads.io/api/device-api/
		{
			"device": "/api/device-api/{serial}/",
			"time"  : ["0x14d95894815", "0x1"],
			"dataPoints":[
						{"wattage": 170},
						{"wattage": 169},
						...
			]
		}

This is a typical packet that could be sent to the server from the device. Here is a breakdown of the fields:

* Device: The hyperlinked device sending the data.

* Time: A tuple of the format `[start_time, period]` in hexadecimal describing the packet's timing in milliseconds.

* dataPoints: An array of undefined size containing measured values for the server. The data points are numbered `0 ... n ... j` where nth data point has a timestamp of `start_time + n * period` and `j` is undefined.





