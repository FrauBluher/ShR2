### Things to keep in mind for ESP development: 
* Set up firmware to configure an internet connection if there is none.
* Fork the esp-httpd project webserver into our repo
* With internet connection, be able to send some garbage data to the server via UART
* Server hardcoded for now, that isn't configured
* Doesn't establish a connection, just sends garbage, and receives the http response

* What this entails: UART send/receive drivers, TCP send/receive drivers

* Starter code began with esp webserver http://git.spritesserver.nl/esphttpd.git/

