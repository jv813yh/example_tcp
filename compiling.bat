@ECHO OFF
ECHO ONLY COMPILATION
ECHO Simple example TCP/IP unsecured communication

gcc tcp_client.c -Wall -Wextra -o2 -o tcp_client -lws2_32
gcc tcp_serve_toupper.c -Wall -Wextra -o2 -o tcp_server -lws2_32 

pause

