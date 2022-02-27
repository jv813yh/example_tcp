/**                                                               **
 * =============================================================== *
 * tcp_serve_toupper.c   v.0.5                                     * 
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
 * (Compileable on Windows with WinLibs standalone build of GCC    * 
 * and MinGW-w64 with make) (-zatial este nie je dodany makefil    * 
 *                                                                 * 
 * Author-Jozef Vendel  Creation date- 20.12.2021                  * 
 * =============================================================== *
 */

/* ======== Includes ======================== */

/* Basic libraries for working in C. */
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* Constants for networking work Windows */
#include "win_linux_sockets.h"

/* ======= Local Macro Definitions ========== */

/* Maximum data size processed by the program */
#define DATA_SIZE  UINT16_MAX

int main() {

    printf("\ntcp_serve_toupper:\n" 
        "receives a client call to establish an unprotected TCP connection,\n" 
        "receives data from the client, calls the toupper() to edit the data,\n" 
        "and sends the modify data back to the client\n");
    printf("\nNo socket multiplexing, it is possible to operate only one\n" 
        "communication without creating other new connections.\n");

/* ======= Global variables ========== */

    char read[DATA_SIZE];   /*<- data from client */

/* ======= Creating client socket and TCP connection ==== */

    /* Initializing Winscok - Win32 apps */
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d)) {
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }

    /* 
     * Configuring remote address with getaddrinfo - 
     * the parameters were entered from the command line 
     * like ip address and port number 
     */
    printf("Configuring local address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    /*
    * ...............................................
    *
    */

    struct addrinfo *bind_address;
    getaddrinfo(0, "8080", &hints, &bind_address);

    /* Creating server socket - socket_listen */
    printf("Creating socket...\n");
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
            bind_address->ai_socktype, bind_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_listen)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }


    printf("Binding socket to local address...\n");
    if (bind(socket_listen,
                bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    /*      ......                    */
    freeaddrinfo(bind_address);

    /* The server is waiting for a client connection attempt */
    printf("\nListening...\n");
    if (listen(socket_listen, 1) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Waiting for connections...\n");

    /* Creating TCP connection with client */
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);
    SOCKET socket_client = accept(socket_listen,
                            (struct sockaddr*) &client_address,
                            &client_len);

    if (!ISVALIDSOCKET(socket_client)) 
    {
        fprintf(stderr, "accept() failed. (%d)\n",
                GETSOCKETERRNO());
        return 1;
    }

    char address_buffer[100];
    getnameinfo((struct sockaddr*)&client_address,
                client_len, address_buffer, sizeof(address_buffer), 0, 0,
                NI_NUMERICHOST);

     /*
     * TCP connection was successfully established in the 
     * client-server architecture, 
     * it is possible to access the data exchange.
     */

    printf("\nNew connection from %s\n", address_buffer);


    while(1) 
    {
        memset(read, 0, sizeof(read));
        
        /* Expected data from the client */
        int bytes_received = recv(socket_client, read, DATA_SIZE, 0);

        printf("Received %d bytes\n", bytes_received);

        /* The checking receive of the data */ 
        if (bytes_received < 1) 
        {
            printf("Connection closed by client :(\n");

            /* Socket from client is closured */
            CLOSESOCKET(socket_client);
            break;
        }

        /* Server service using the toupper() */
        for (int j = 0; j < bytes_received; ++j)
            read[j] = toupper(read[j]);

        /* Sending modify data */
        int bytes_sent = send(socket_client, read, bytes_received, 0);
        printf("Sent %d bytes.\n", bytes_sent);

        /* The checking send of the data */ 
        if (bytes_sent < 1) 
        {   
            printf("Connection closed by client :(\n");

            /* Socket from client is closured */
            CLOSESOCKET(socket_client);

            break;
        }  

    } /* end while() {...} */

    /* Exiting the program  */
    printf("Closing listening socket...\n");
    CLOSESOCKET(socket_listen);

    WSACleanup();

    printf("Finished.\n");
    return 0;
}
