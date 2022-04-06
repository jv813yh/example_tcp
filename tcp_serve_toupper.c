/**                                                               **
 * =============================================================== *
 * tcp_serve_toupper.c   v.0.7                                     * 
 *                                                                 * 
 * KEMT FEI TUKE, Diploma thesis                                   * 
 *                                                                 *
 * tcp_serve_toupper:                                              *
 * receives a client call to establish an unprotected              * 
 * TCP connection, receives data from the client,                  * 
 * calls the toupper() to edit the data,                           * 
 * and sends the modify data back to the client                    *
 *                                                                 * 
 * No socket multiplexing, it is possible to operate only one      * 
 * duplex  communication without creating other new connections.   * 
 *                                                                 * 
 * Compileable on Windows with WinLibs standalone build of GCC     * 
 * and MinGW-w64                                                   * 
 *                                                                 * 
 * Compileable on Linux with GCC                                   *
 *                                                                 * 
 * Author-Jozef Vendel  Creation date- 20.12.2021                  * 
 * =============================================================== *
 */

/* ======== Includes ======================== */

/* Basic libraries for working in C. */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

/* Macros for networking work Windows / Linux */
#include "win_linux_sockets.h"

/* ======= Local Macro Definitions ========== */

/* Maximum data size processed by the program */
#define DATA_SIZE  UINT16_MAX * 30

/* Auxiliary static fields of size ...*/
#define AUXILIARY_FIELD_SIZE    100

/*======= Global function implementation ===========*/

/* 
 * Function creating a socket (return this socket) for the server and 
 * expects a connection from the client.
 * The arguments of the function are the IP addressand the port number
 * of the server. 
 *
 * Function parameters are arguments from the command line.
 */
SOCKET create_socket_and_listen(const char* host, const char *port);

int main(int argc, char *argv[]) {

    /* Checking the correct program parameters */
    if (argc < 3) {
        fprintf(stderr, "usage: tcp_server ip_host port\n");
        return 1;
    }
/* ======= Bacis information about program ========== */
    printf("\n*******************************************************************\n");
    printf("* tcp_serve_toupper:                                              *\n" 
        "* receives a client call to establish an unprotected TCP          *\n" 
        "* connection, receives data from the client, calls the toupper()  *\n" 
        "* to edit the data and sends the modify data back to the client   *\n");
    printf("* No socket multiplexing, it is possible to operate only one      *\n" 
        "* communication without creating other new connections.           *\n");
    printf("*******************************************************************\n");

/* ======= Global variables ========== */

    /* 
    * If the TCP connection to the client is successfully established, 
    * the variable start is incremented and the data exchange 
    * is accessed in the cycle ... while(start){....}
    */
    int32_t start = 0;

    /* Variables for obtaining the size of sent, received data */
    int32_t bytes_sent, bytes_received;

    uint8_t read[DATA_SIZE];   /*<- data from client */

    /* 
     * socket_listen for host
     * socket_client for client
     *
     */
    SOCKET socket_listen, socket_client;

/* ======= Creating host socket and TCP connection ==== */

    /* Initializing Winscok - Win32 apps , ONLY FOR WINDOWS !!! */
#if defined(_WIN32)
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d)) {
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }
#endif

    /* 
     * Creates a socket (return this socket) with which it waits 
     * for a connection from the client   
     *
     */
    socket_listen = create_socket_and_listen(argv[1], argv[2]);

    printf("Waiting for connections with client...\n");

    /* Creating TCP connection with client */
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);
    socket_client = accept(socket_listen,
                          (struct sockaddr*) &client_address,
                          &client_len);

    /* Verification of the correct establishment of the connection */
    if (!ISVALIDSOCKET(socket_client)) 
    {
        fprintf(stderr, "accept() failed. (%d)\n",
                GETSOCKETERRNO());
        return 1;
    } else 
    {
        /* Listing the ip address of the client */
        char address_buffer[AUXILIARY_FIELD_SIZE];
        int return_getnameinfo;
        return_getnameinfo = getnameinfo((struct sockaddr*)&client_address,
                client_len, address_buffer, sizeof(address_buffer), 0, 0,
                NI_NUMERICHOST);

        if (return_getnameinfo)
        {
            fprintf(stderr, "getnameinfo() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }

    /*
     * TCP connection was successfully established in the 
     * client-server architecture, 
     * it is possible to access the data exchange.
     */
        printf("\nNew connection from %s\n\n", address_buffer);

        /* We can exchange data */
        start++;
    }

/* ======= Unsecured data exchange cycle ==== */
    while(start) 
    {
        memset(read, 0, sizeof(read));
        
        /* Expected data from the client */
        bytes_received = recv(socket_client, (char *)read, DATA_SIZE, 0);

        /* The checking receive of the data */ 
        if (bytes_received <= 0) 
        {
            printf("\nConnection closed by client :(\n");

            /* Socket from client is closured */
            CLOSESOCKET(socket_client);
            break;
        }

        printf("Received %u bytes\n", bytes_received);

        /* Server service using the toupper() */
        for (int32_t j = 0; j < bytes_received; ++j)
            read[j] = toupper(read[j]);

        /* Sending modify data */
        bytes_sent = send(socket_client, (char *)read, bytes_received, 0);

        /* The checking send of the data */ 
        if (bytes_sent <= 0) 
        {   
            printf("\nConnection closed by client :(\n");

            /* Socket from client is closured */
            CLOSESOCKET(socket_client);

            break;
        }  

        printf("Sent %u bytes.\n", bytes_sent);

    } /* end while() {...} */

/* ========  End of application  ========================= */
    printf("Closing listening socket...\n");

    /* Drawer closure */
    CLOSESOCKET(socket_listen);

    /* Cleanning Winsock, ONLY FOR WINDOWS !!! */
#if defined(_WIN32)
    WSACleanup();
#endif

    printf("Finished application :)\n\n");
    
    return 0;
}

/* 
 * Creates a socket (return this socket) and expects 
 * a connection from the client.
 * Function parameters are arguments from the command line.
 *
 * @par ip_server: ip adrress server 
 * @par port: number of port
 *
 * return server socket and expects a connection from the client.
 * return wrong report in case failure
 */

SOCKET create_socket_and_listen(const char* host, const char *port) 
{
    printf("\nConfiguring local address...\n");

    /* 
     * Configuring remote address with getaddrinfo - 
     * the parameters were entered from the command line 
     * like ip address and port number 
     */
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    if (getaddrinfo(host, port, &hints, &bind_address))
    {
        fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    /* Creating server socket - socket_listen */
    printf("Creating socket...\n");
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
            bind_address->ai_socktype, bind_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_listen)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        exit(1);
    }

    printf("Binding socket to local address...\n");
    if (bind(socket_listen,
                bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        exit(1);
    }

    freeaddrinfo(bind_address);

    /* The server is waiting for a client connection attempt */
    printf("Listening...\n\n");
    if (listen(socket_listen, 1) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
        exit(1);
    }

    return socket_listen;
}
