.. _testing:

Testing
=======

There are several facets to this project that can be tested in a variety
of ways. Because of how different each module is, they all have individual
ways in which they are stressed.

Elastic Load Balancer and Auto Scaling Group
--------------------------------------------

The most forward-facing module to the framework is the ELB and Auto
Scaling Group. These two appliances have the responsibility of 
scaling the framework up or down based on necessity.

The command line tool `siege` can be used to simulate a flood of requests
to the server framework.

Siege should be run on a system other than the stateless web server, and
can be installed with the package manager on your system::

    sudo apt-get install siege

A good test for this framework is the following::

    $ siege -c100 -t20M seads.io

This gives the Auto Scaling Group four chances to instantiate new
instances. The 100 concurrent connections will assure that our
tiny instance is overloaded.