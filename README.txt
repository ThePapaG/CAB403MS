CAB403MS
Cab403 final assignment mine sweeper server and client

Compilation and Run
An appropriate makefile structure has been written for the different components.

To make the assignment as a whole locate the root directory and run:

make

This will make server.exe and client.exe in the bin directory. To run these components please execute the command line tools;

Server

./bin/server.exe PORT

Where PORT is the port you wish to open the server on.

Client

./bin/client.exe HOST_IP PORT

Where HOST_IP is the ip of the server host (localhost if on the same machine), and PORT is the port that the client is to connect to the server on.