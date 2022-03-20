/**                                                            **
 * ============================================================ *
 * tcp_client.c   v.0.9                                         *   
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
 * and MinGW-w64       
 *
 * Compileable on Linux with ....                                         *
 *                                                              *
 * Author-Jozef Vendel  Creation date- 20.12.2021               *
 * ============================================================ *
 */ 

/* ======== Includes ======================== */

/* Basic libraries for working in C. */
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* Macros for networking work Windows / Linux */
#include "win_linux_sockets.h"

/* ======= Local Macro Definitions ========== */

/* Maximum data size processed by the program */
#define DATA_SIZE  UINT16_MAX * 30

/* Auxiliary static fields of size ...*/
#define AUXILIARY_FIELD_SIZE    100

/*======= Global function implementation ===========*/

/* 
 * Function creating a socket (return this socket) for the client and 
 * try to connect to the server. 
 * The arguments of the function are the IP address of the server and 
 * the port number on which the server establishes new connections.
 *
 * Function parameters are arguments from the command line.
 */
SOCKET create_socket_and_connect(const char* ip_server, const char *port);


int main(int argc, char *argv[]) {

    /* Checking the correct program parameters */
    if (argc < 3) {
        fprintf(stderr, "usage: tcp_client ip_host port\n");
        return 1;
    }

/* ======= Bacis information about program ========== */
    printf("\n*************************************************************\n");
    printf("* tcp_client:                                               *\n"
        "* Establishes an unprotected TCP connection to the server,  *\n"
        "* sends data from the CLI from the client,                  *\n"
        "* and expects exchange data to be received back.            *\n");
    printf("* No socket multiplexing,it's possible to operate only      *\n" 
        "* one communication without creating other new connections. *\n");
    printf("*************************************************************\n");

/* ======= Global variables ========== */

    uint8_t input_data[DATA_SIZE], /*<- input data from the client */
            finish;                /*<- the character that decides
                                    * about end of the program 
                                    */
   /* 
    * If the TCP connection to the server is successfully established, 
    * the variable start is incremented and the data exchange 
    * is accessed in the cycle ... while(start){....}
    */
    int32_t start = 0;

    /* Variables for obtaining the size of sent, received data */
    int32_t bytes_sent, bytes_received;

    /* Client socket for TCP */
    SOCKET client_socket;

/* ======= Creating client socket and TCP connection ==== */

    /* Initializing Winscok - Win32 apps , ONLY FOR WINDOWS !!! */
#if defined(_WIN32)
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d)) {
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }
#endif

    /* 
     * It will create a socket with which the client will 
     * connect to the server
     */
    client_socket = create_socket_and_connect(argv[1], argv[2]);

    /* Connection control */
    if (ISVALIDSOCKET(client_socket))
    {   
        /* We can exchange data */
        start++;
        printf("\nConnection to the server :)\n");
    } 
    else printf("\nError connecting to server :(\n");

    /*
     * TCP connection was successfully established in the 
     * client-server architecture. 
     * It is possible to access the data exchange.
     */
/* ======= Unsecured data exchange in cycle ==== */
    while(start) 
    {
        /* Expected data from the client from the CLI */
        printf("\nTo send data, enter text followed by enter.\n");
        memset(input_data, 0, sizeof(input_data));

        /*
         * Retrieving data from the client (CLI)
         * Return value testing fgets() 
         */
        if (!fgets((char *)input_data, DATA_SIZE, stdin)) break;

        /* Sending data */
        printf("\nSending: %s", input_data);
        bytes_sent = send(client_socket, (char *)input_data, strlen((char *)input_data), 0);

        /* The checking send of the data */ 
        if (bytes_sent < 0) 
        {
            printf("\nConnection closed by server :(\n");
            break;
        }
        /* List of sent size data */
        printf("Sent %u bytes\n", bytes_sent);
        
        /* Receiving data from server */
        memset(input_data, 0, sizeof(input_data));

        bytes_received = recv(client_socket, (char *)input_data, DATA_SIZE, 0);

        /* The checking receive of the data */ 
        if (bytes_received < 0) 
        {
            printf("\nConnection closed by server :(\n");
            break;
        }
        /* List of received size data */
        printf("Received %u bytes: %.*s\n",
                        bytes_received, bytes_received, input_data);

        /* Deciding whether to terminate the while() -> program */                       
        printf("\nDo you want to finish?\nPress any key, 'q' to quit\n");

        finish = getchar();

        if(finish == 'q') break;

        /* Buffer cleaning */
        while(getchar() != '\n')
            ;

    } /* end while() {...} */
    
/* ========  End of application  ========================= */
    printf("Closing client socket ...\n");

    /* Drawer closure */
    CLOSESOCKET(client_socket);

    /* Cleanning Winsock, ONLY FOR WINDOWS !!! */
#if defined(_WIN32)
    WSACleanup();
#endif

    printf("Finished application :)\n\n");
    
    return 0;
}

/* 
 * Create a socket with which the client will connect to the server.
 * Function parameters are arguments from the command line.
 *
 * @par ip_server: ip adrress server 
 * @par port: number of port
 *
 * return client socket with connection to the server in case success
 * return wrong report in case failure
 */
SOCKET create_socket_and_connect(const char* ip_server, const char *port) 
{
    printf("\nConfiguring remote address...\n");

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *peer_address;
    
    if (getaddrinfo(ip_server, port, &hints, &peer_address))
    {
        fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Remote address is: ");
    char address_buffer[AUXILIARY_FIELD_SIZE];
    char service_buffer[AUXILIARY_FIELD_SIZE];

    int return_getnameinfo;
    return_getnameinfo = getnameinfo(peer_address->ai_addr, 
            peer_address->ai_addrlen,
            address_buffer, sizeof(address_buffer),
            service_buffer, sizeof(service_buffer),
            NI_NUMERICHOST);

    if (return_getnameinfo)
    {
        fprintf(stderr, "getnameinfo() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
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

    freeaddrinfo(peer_address);

    return client_socket;
}
