# simple_example_tcp
The example_tcp folder contains a simple example of a server and a client
which They communicate together using the TCP communication channel.
The server works on port 8080. 
A client that knows the IP address
server and the port number on which the server is listening will connect to the server.
The client enters this information when calling the application.
The client sends the data entered from the command line.
The server provides a service that translates lowercase letters in the text
sent to the client in capital letters (with toupper())
and sent back to the client.
The client displays the received data on the screen.

No dynamic memory allocation, no socket multiplexing,
it is possible to operate only one  
duplex communication without creating other new connections.

If you want cryptographically secured communication:
https://github.com/jv813yh/simple_example_tcp_salt_channel

# Windows: 
with .bat files or command line

# Linux:
gcc tcp_client.c -Wall -Wextra -o2 -o tcp_client

gcc tcp_serve_toupper.c -Wall -Wextra -o2 -o tcp_server


