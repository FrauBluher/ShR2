.. _installation:

Installation
============

Introduction
------------

The SEADS web infrastructure provieds a simple API to read/write the data gathered by the SEAD Light.In addition, this framework is designed to provide in-house visualization and cost analysis of the energy usage. Built on Django, it is encouraged that this environment be augmented with new and improved applications that can interface with the data.

The current setup has all the necessary infrastructure in place for SEAD Lights to send their data. In addition, a proof-of-concept application has been created to interface with the data in a meaningful way.

There are two ways to deploy this system:

1. Scalable framework based in the Amazon Cloud Computing Services (recommended)
2. Atomic install on a single machine (easy)

Installing Scalable Framework in Amazon Compute Cloud (Ubuntu)
--------------------------------------------------------------

.. Note::

	This project was developed entirely on a Ubuntu build (14.04.2) so these instructions will be tailored towards that build. However, this framework should install semi-peacefully on any OS that can run python/nginx/uwsgi.

	These instructions will assume you know how to interface with the amazon AWS console to create new instances. Refer to the `Getting Started Guide <http://docs.aws.amazon.com/AWSEC2/latest/UserGuide/EC2_GetStarted.html>`_ for more information.

Basic Server Outline
~~~~~~~~~~~~~~~~~~

For the scalable framework to work correctly, there is a bare minimum of 3 servers that need to be always running. Each server has a unique purpose:

1. Influxdb Server - A medium/large instance that houses the data from the SEAD Light. Needs to be large to handle the calculations that go into the fanout queries of the database.
2. Webapp Stateful Server - A server that houses the Django database that holds state information about the web application, such as user credentials and model relations.
3. Webapp Stateless Server - A skeleton server that serves as the frontend for users connecting to the web interface. This server is a clone of an image used in the auto scaling group.

The servers are setup with the following hierarchy:

.. image:: http://i.imgur.com/SCdI31d.png
	:align: center

The infrastructure is set up in this way to deal with a scalable load in an intelligent way. If there is little to no traffic to the website/REST API, the servers will spin down to a minimal state. However, if load increases, the infrastructure is designed to automatically spin up new instances of the web application so that no single instance is overloaded.


InfluxDB Server Setup
~~~~~~~~~~~~~~~~~~~~~

To get started, create a medium/large instance for the InfluxDB database. The operating system is recommended to be Ubuntu, but this is not a requirement. This server will only be interfaced by the stateless web servers under the following circumstances: 1) A user requests device data via the web application, 2) A user/device interacts with the REST API to read/write device data.

The following ports should be opened for the InfluxDB instance:

+-----------------+----------+------------+-----------+---------------------------------------------------------+
| Type            | Protocol | Port Range | Source    | Purpose                                                 |
+=================+==========+============+===========+=========================================================+
| SSH             | TCP      | 22         | 0.0.0.0/0 | Self explanatory                                        |
+-----------------+----------+------------+-----------+---------------------------------------------------------+
| Custom TCP Rule | TCP      | 8083       | 0.0.0.0/0 | Exposes the database web API for interactive use        |
+-----------------+----------+------------+-----------+---------------------------------------------------------+
| Custom TCP Rule | TCP      | 8086       | 0.0.0.0/0 | Exposes the database REST port for the python interface |
+-----------------+----------+------------+-----------+---------------------------------------------------------+

Once the server has booted, connect to it via ssh and do the following:

1. Install Git::
	
	sudo apt-get install git

2. Clone the SEADS repository::
	
	git clone https://github.com/Fraubluher/ShR2/

3. Run the deploy script for influxdb::

	cd ShR2/Web\ Stack/
	sudo ./deploy_database.sh

4. Reboot the server::

	sudo reboot

5. Configure the database::

	curl -X POST 'http://localhost:8086/db?u=<username>&p=<password>' \
  		 -d '{"name": "seads"}'

The InfluxDB server is now ready to respond to requests.



Django Web Application Stateful Server Setup
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This process will walk you through the process of installing a stateful server for the Django web application.

Create a tiny/small instance running Ubuntu (the operating system is recommended to be Ubuntu, but this is not a requirement).

The following ports should be opened for the stateful server:

+-----------------+----------+------------+-----------+---------------------------------------------------------+
| Type            | Protocol | Port Range | Source    | Purpose                                                 |
+=================+==========+============+===========+=========================================================+
| SSH             | TCP      | 22         | 0.0.0.0/0 | Self explanatory                                        |
+-----------------+----------+------------+-----------+---------------------------------------------------------+
| MYSQL           | TCP      | 3306       | 0.0.0.0/0 | Port for remotely interfacing with Django database      |
+-----------------+----------+------------+-----------+---------------------------------------------------------+

Once the server has booted, connect to it via ssh and do the following:

1. Install Git::
	
	sudo apt-get install git

2. Clone the SEADS repository::
	
	git clone https://github.com/Fraubluher/ShR2/

3. Run the deploy script for influxdb::

	cd ShR2/Web\ Stack/
	sudo ./deploy_webapp_stateful.sh

This script will take you through the process of creating the MySQL database to be used by the stateless servers in the future. You will be prompted to create a root user on the database, remember the credentials for later.

This script will install all the necessary dependencies for the Django project. This will take a while, grab a beverage.

Near the end, several prompts will appear. You will be prompted to create the Django user in the MySQL database that is used to interface with the stateless servers. Leaving prompts blank will roll over to their default values indicated in the parentheses.

4. Reboot the server::

	sudo reboot

This server should now be properly configured to run as a stateful implementation of the web application.


Django Web Application Stateless Server Setup
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The final step in assembling the server infrastructure is to create a stateless instance of the web application. This will provide the basis for which an auto scaler can instantiate more/less instances of the web application automatically.


Create a tiny/small instance running Ubuntu (the operating system is recommended to be Ubuntu, but this is not a requirement).

The following ports should be opened for the stateful server:

+-----------------+----------+------------+-----------+---------------------------------------------------------+
| Type            | Protocol | Port Range | Source    | Purpose                                                 |
+=================+==========+============+===========+=========================================================+
| SSH             | TCP      | 22         | 0.0.0.0/0 | Self explanatory                                        |
+-----------------+----------+------------+-----------+---------------------------------------------------------+
| HTTP            | TCP      | 80         | 0.0.0.0/0 | Self explanatory                                        |
+-----------------+----------+------------+-----------+---------------------------------------------------------+

Since this is the forward-facing instance, the HTTP port is opened for clients to connect to. This allows both end users and SEAD Lights to connect and interact.

Once the server has booted, connect to it via ssh and do the following:

1. Install Git::
	
	sudo apt-get install git

2. Clone the SEADS repository::
	
	git clone https://github.com/Fraubluher/ShR2/

3. Run the deploy script for influxdb::

	cd ShR2/Web\ Stack/
	sudo ./deploy_webapp_stateless.sh

When this script runs, it will prompt for the address for the remote database (Django database host address). This is the address of the server created in the previous step.

4. Reboot the server::

	sudo reboot

When the server reboots, you should now be able to connect to it from a web browser and test out the functionality. The stateless server is the address in which clients and SEAD Lights should connect.


Finishing Up
~~~~~~~~~~~~

At this point, you have a functioning server framework that is eligible for load balancing and auto scaling. This guide does not get into the specifics since it is unique to the cloud service being used.

In general, these are the steps you should follow:

1. Create an image from the fully-configured webapp stateful server.
2. Configure and auto scaling group based on the image.
3. Configure a load balancer based off the auto scaling group.

If you choose to link the server's address to a domain name after configuring a load balancer, a CNAME record must be created with the DNS provider with the load balancer's address.


Installing Atomic Server
------------------------

.. Note::

	This project was developed entirely on a Ubuntu build (14.04.2) so these instructions will be tailored towards that build. However, this framework should install semi-peacefully on any OS that can run python/nginx/uwsgi.

	These instructions will not focus on deploying in the Amazon Compute Cloud, however it is certainly possible to do so.

Basic Server Outline
~~~~~~~~~~~~~~~~~~~~

This server will comprise all aspects of the project on a single machine. This type of setup is intended for a small user base on the order of 10's of users. Any more and you should consider adopting the scalable approach above. It is recommended to use Ubuntu simply because this platform was developed and tested solely on Ubuntu.

To get started, open up the following ports on your machine:

+-----------------+----------+------------+-----------+---------------------------------------------------------+
| Type            | Protocol | Port Range | Source    | Purpose                                                 |
+=================+==========+============+===========+=========================================================+
| SSH             | TCP      | 22         | 0.0.0.0/0 | Self explanatory                                        |
+-----------------+----------+------------+-----------+---------------------------------------------------------+
| Custom TCP Rule | TCP      | 8083       | 0.0.0.0/0 | Exposes the database web API for interactive use        |
+-----------------+----------+------------+-----------+---------------------------------------------------------+
| Custom TCP Rule | TCP      | 8086       | 0.0.0.0/0 | Exposes the database REST port for the python interface |
+-----------------+----------+------------+-----------+---------------------------------------------------------+
| MYSQL           | TCP      | 3306       | 0.0.0.0/0 | Port for remotely interfacing with Django database      |
+-----------------+----------+------------+-----------+---------------------------------------------------------+
| HTTP            | TCP      | 80         | 0.0.0.0/0 | Self explanatory                                        |
+-----------------+----------+------------+-----------+---------------------------------------------------------+

1. Install Git::
	
	sudo apt-get install git

2. Clone the SEADS repository::
	
	git clone https://github.com/Fraubluher/ShR2/

3. Run the deploy script for influxdb::

	cd ShR2/Web\ Stack/
	sudo ./deploy_webapp_stateful.sh

This script will walk you through creating and configuring the databases needed. For any prompt asking for an address, enter 'localhost'.

4. Reboot the server::

	sudo reboot

When the server reboots, verify it works by visiting the server from a webpage. All basic functionality should now exist.
