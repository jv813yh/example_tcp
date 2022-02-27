@ECHO OFF
ECHO ONLY COMPILATION
ECHO Source code taken from:
ECHO https://github.com/PacktPublishing/Hands-On-Network-Programming-with-C/tree/master/chap03
ECHO TCP communication channel with loopback 

gcc tcp_client.c -o tcp_client -lws2_32
gcc tcp_serve_toupper.c -o tcp_server -lws2_32 

pause

