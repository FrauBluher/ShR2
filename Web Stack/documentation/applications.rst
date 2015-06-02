.. _applications:

Applications
============

Three distinct applications were created that encompass the SEAD web framework:

1. Microdata - Interfaces with the devices
2. Webapp - Interfaces with the clients
3. Farmer - Manages the devices

And (optionally) a fourth:

4. Debug - Models that assist in debugging the project

The applications were designed such that the Webapp application is hot-swappable. Since it is a proof of concept, it can either be added to or replaced altogether. Microdata and Farmer are able to run without Webapp, however their data cannot be interfaced with in a meaningful way without a web application.

.. image:: http://i.imgur.com/uw3BtWt.png
	:align: center

`Full Size <http://i.imgur.com/uw3BtWt.png>`_

What follows is the breakdown of each application and how they work.

.. toctree::
	:maxdepth: 2

	modules/microdata
	modules/webapp
	modules/farmer

