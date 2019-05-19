// Mateusz Siłaczewski IZ06IO1 16084

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


void receive_data (int socket)
{
    int data;
    char buffer[BUFF + 1];
    data = recv(socket, buffer, BUFF,0);
    buffer[data] = 0;
    fprintf(stdout,"Received messsage %s\n", buffer);
}

int create_connection (int socket, const char * ip, int port, SOCKET_TYPE type)
{
    if (type == TCP)
    {
        printf("Attempting TCP connection");
        struct sockaddr_in address;
        
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr(ip);
        address.sin_port = htons(port);
        memset( &( address.sin_zero ), '\0', 8 );

        printf("Connecting to %s:%d...\n", ip, port);

        int state = connect(socket, (struct sockaddr *) & address, sizeof(struct sockaddr));

        if (state == -1)
        {
            fprintf(stderr, "Failed to connect to socket, id: %d: %s\n", socket, strerror(errno));
            exit(EXIT_FAILURE);
            return state;
        }

        printf("Connected with socket, id: %d\n", socket);

        return state;
    }
    //todo
    return -1;
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

void send_message_int(int socket, int message, int message_size){
    void * buff = calloc(1,message_size+22);
    snprintf(buff,message_size+22,"PRE%dSUF",message);
    ssize_t len = send(socket, buff, message_size+22, 0);
        if (len < 0)
        {
              fprintf(stderr, "Error on sending greetings --> %s", strerror(errno));

              exit(EXIT_FAILURE);
        }
    
    free(buff);
}

void send_message_char(int socket, char* message, int message_size){
    char * buff = calloc(1,message_size+22);
    snprintf(buff,message_size+22,"PRE%sSUF",message);
    ssize_t len = send(socket, buff, message_size+22, 0);
    fprintf(stdout,"SENDING %s",buff);
        if (len < 0)
        {
              fprintf(stderr, "Error on sending greetings --> %s", strerror(errno));

              exit(EXIT_FAILURE);
        }
    
    free(buff);
}



void send_frames(int socket, int* frames, int items_to_send, int size_of_frame, int frames_in_message){
    char * buff = calloc(1,size_of_frame*frames_in_message);
    char * buff_temp = calloc(1,size_of_frame);

    
    for(int i = 0; i< items_to_send-frames_in_message;){
        for(int j=0;j< frames_in_message;j++){
            snprintf(buff_temp,size_of_frame,"PRE%dSUF",frames[i+j]);
            strcat(buff,buff_temp);

        }
        fprintf(stdout,"Sending frame %d out of %d, content %s\n",i,items_to_send,buff);
        int write_result = send(socket,buff,size_of_frame*frames_in_message,0);
        if (write_result < 0){
            fprintf(stderr,"Failed to send frame %d, write result: %d\n",i,write_result);
            exit(1);
        };
        i = i+frames_in_message;
        buff[0]='\0';
        buff_temp[0]='\0';
    }
    free(buff);
    free(buff_temp);

}

void send_time (int socket)
{
    char buffer[BUFF + 1];
    time_t this_time;

    this_time = time(NULL);
    snprintf(buffer, BUFF, "%s\n", ctime(&this_time));

    write(socket, buffer, strlen(buffer));
}

void send_uptime (int socket)
{
    char buffer[BUFF + 1];
    FILE *fp;
    char path[500];

    /* Open the command for reading. */
    fp = popen("/bin/uptime /etc/", "r");
    if (fp == NULL)
    {
        printf("Failed to run command\n" );
        exit(1);
    }
    fgets(path, sizeof(path)-1, fp);

    pclose(fp);
    snprintf(buffer, BUFF, "%s\n", path);
    write(socket, buffer, strlen(buffer));
}

int disconnect (int socket)
{
    int status = close(socket);
    if (status == -1)
    {
        fprintf(stderr, "Failed to close connection for socket, id: %d: %s\n", socket, strerror(errno));
        exit(EXIT_FAILURE);
        return status;
    }
    printf("Closing connection with socket, id: %d\n", socket);

    return status;
}
