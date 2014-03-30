A simple (mostly Java) example of a LabComm channel over TCP

The class labcommTCPtest/Example.java contains set up of a server
and a client, and can be run by 'make run' 

Also included is a generic decoder in python, and a server that only sends
one sample to the first client that connects, and then closes the connection
and exits.

Those can be run by

* first, in one terminal: make runOSserver
* then, in another terminal: ./runpy localhost 9999

