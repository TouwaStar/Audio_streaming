#ifdef _WIN32
    #include <winsock2.h>
    #include <WS2tcpip.h>
#else
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <unistd.h>
    #include <syslog.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <limits.h>


#ifndef ssize_t
    #if SIZE_MAX == UINT_MAX
    typedef int ssize_t;        /* common 32 bit case */
    #elif SIZE_MAX == ULONG_MAX
    typedef long ssize_t;       /* linux 64 bits */
    #elif SIZE_MAX == ULLONG_MAX
    typedef long long ssize_t;  /* windows 64 bits */
    #elif SIZE_MAX == USHRT_MAX
    typedef short ssize_t;      
    #endif
#endif


typedef enum {TCP=SOCK_STREAM, UDP=SOCK_DGRAM} SOCKET_TYPE;
#define FALSE 0
#define TRUE 1

#define BUFF 512

int create_socket (SOCKET_TYPE type)
{
    int socket_id;
    socket_id = socket(AF_INET, type, 0);
    if (socket_id == -1)
    {
        fprintf(stderr, "Failed to create socket %s\n", strerror(errno));
        exit(EXIT_FAILURE);
        return socket_id;
    }
    printf("Created socket, id: %d\n", socket_id);
    return socket_id;
}

int bind_port (int socket, int port)
{
    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);
    memset( &( address.sin_zero ), '\0', 8 );

    int status = bind(socket, (struct sockaddr *) & address, sizeof(struct sockaddr));

    if (status == -1)
    {
        fprintf(stderr, "Failed to bind port %d: %s\n", port, strerror(errno));
        exit(EXIT_FAILURE);
        return status;
    }

    printf("Binded port %d\n", port);

    return status;
}

int accept_connection (int socket)
{
    unsigned short port;
    struct in_addr client_ip;
    char * ip;
    struct sockaddr_in address;
    socklen_t address_len = sizeof(struct sockaddr_in);


    fprintf(stdout,"Awaiting connection...\n");

    int peer_socket = accept(socket, (struct sockaddr *) &address, &address_len);
    client_ip = address.sin_addr;
    ip = inet_ntoa(client_ip);
    port = ntohs(address.sin_port);

    if (peer_socket == -1)
    {
        fprintf(stderr, "Failed to accept connection with client %s:%d: %s\n", ip, port, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Accepted connection with client  %s:%d\n", ip, port);

    return peer_socket;
}

int listen_to_socket (int socket, int log, SOCKET_TYPE type)
{
    if(type == TCP)
    {
        int listen_status = listen(socket, log);

        if (listen_status == -1)
        {
            fprintf(stderr, "Failed to bind socket, id: %d with log %d: %s\n", socket, log, strerror(errno));
            exit(EXIT_FAILURE);
            return listen_status;
        }

        printf("Listening to socket, id: %d with log %d\n", socket, log);

        return listen_status;
    }
    //todo
    return -1;
}

