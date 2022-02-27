/**                                                            **
 * ============================================================ *
 * tcp_client.c   v.0.7                                         *   
 *                                                              *
 * KEMT FEI TUKE, Diploma thesis                                *
 *                                                              *
 * tcp_client:                                                  *
 * establishes an unprotected TCP connection to the server,     *
 * sends data from the CLI from the client,                     *
 * and expects data to be received back from the server         *
 *                                                              *
 * No socket multiplexing, it is possible to operate only one   *
 * duplex communication without creating other new connections. *
 *                                                              *
 * Compileable on Windows with WinLibs standalone build of GCC  *
 * and MinGW-w64 with make ...nie je dodany makefile subor !!!  *
 *                                                              *
 * Author-Jozef Vendel  Creation date- 20.12.2021               *
 * ============================================================ *
 */ 

/* ======== Includes ======================== */

/* Basic libraries for working in C. */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <conio.h>

/* Constants for networking work Windows */
#include "win_linux_sockets.h"

/* ======= Local Macro Definitions ========== */

/* Maximum data size processed by the program */
#define DATA_SIZE  UINT16_MAX

int main(int argc, char *argv[]) {

/* ======= Bacis information about program ========== */

    printf("\ntcp_client:\n"
        "establishes an unprotected TCP connection to the server,\n"
        "sends data from the CLI from the client,\n"
        "and expects data to be received back from the server\n\n");
    printf("\nNo socket multiplexing, it is possible to operate only one\n" 
        "communication without creating other new connections.\n");

/* ======= Global variables ========== */

    char input_data[DATA_SIZE], /*<- input data from client */
        finish;                 /*<- the character that decides
                                 * about end of the program */

/* ======= Creating client socket and TCP connection ==== */

    /* Initializing Winscok - Win32 apps */
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d)) {
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }

    /* Checking the correct program parameters */
    if (argc < 3) {
        fprintf(stderr, "usage: tcp_client hostname port\n");
        return 1;
    }

    /* 
     * Configuring remote address with getaddrinfo - 
     * the parameters were entered from the command line 
     * like ip address and port number 
     */
    printf("\nConfiguring remote address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *peer_address;
    if (getaddrinfo(argv[1], argv[2], &hints, &peer_address)) {
        fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    /*
    * ...............................................
    *
    */


    printf("Remote address is: ");
    char address_buffer[100];
    char service_buffer[100];
    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
            address_buffer, sizeof(address_buffer),
            service_buffer, sizeof(service_buffer),
            NI_NUMERICHOST);
    printf("%s %s\n", address_buffer, service_buffer);

    /* Creating client socket - client_socket */
    printf("\nCreating client socket...\n");
    SOCKET client_socket;
    client_socket = socket(peer_address->ai_family,
            peer_address->ai_socktype, peer_address->ai_protocol);
    if (!ISVALIDSOCKET(client_socket)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    /* Creating TCP connection with server */
    printf("Connecting...\n");
    if (connect(client_socket,
                peer_address->ai_addr, peer_address->ai_addrlen)) {
        fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    /*      ......                    */
    freeaddrinfo(peer_address);

    /*
     * TCP connection was successfully established in the 
     * client-server architecture, 
     * it is possible to access the data exchange.
     */
    printf("\nConnected.\n");

    int start = 1;
    while(start) 
    {
        /* Expected data from the client from the CLI */
        printf("To send data, enter text followed by enter.\n");
        memset(input_data, 0, sizeof(input_data));

        /* Return value testing fgets() */
        if (!fgets(input_data, DATA_SIZE, stdin)) break;

        /* Sending data */
        printf("Sending: %s", input_data);
        int bytes_sent = send(client_socket, input_data, strlen(input_data), 0);
        printf("Sent %d bytes.\n", bytes_sent);

        /* The checking send of the data */ 
        if (bytes_sent < 1) 
        {
            printf("Connection closed by server :(\n");
            break;
        }
        
        /* Receiving data from server */
        memset(input_data, 0, sizeof(input_data));
        int bytes_received = recv(client_socket, input_data, DATA_SIZE, 0);
        printf("Received (%d bytes): %.*s",
                        bytes_received, bytes_received, input_data);

        /* The checking receive of the data */ 
        if (bytes_received < 1) 
        {
            printf("Connection closed by server :(\n");
            break;
        }
        
        /* Deciding whether to terminate the while() -> program */                       
        printf("\nDo you want to finish?\nPress any key, 'q' to quit\n");

        finish = getch();

        if(finish == 'q') break;

    } /* end while() {...} */
    
    /* Exiting the program  */
    printf("Closing client socket...\n");
    CLOSESOCKET(client_socket);

    WSACleanup();

    printf("Finished.\n");
    return 0;
}

