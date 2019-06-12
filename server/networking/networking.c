#include "./networking_basic.c"

#define LEN_OF_APPENDIX 7
#define BUFF 512

void receive_data (int socket)
{
    int data;
    char buffer[BUFF + 1];
    data = recv(socket, buffer, BUFF,0);
    buffer[data] = 0;
    fprintf(stdout,"Received messsage %s\n", buffer);
}

/**
 * Retrieves a message from specified socket and returns a char
 *  pointer to dynamically allocated memory storing the message.
 */
char* retrieve_message_dynamic(int socket){
    int data;
    char* buffer = calloc(1,BUFF+1);
    data = recv(socket, buffer, BUFF,0);
    if (data == 0 || data == -1){
        free(buffer);
        return "CLOSED";
    }
    buffer[data] = 0;
    fprintf(stdout,"Received messsage %s\n", buffer);
    return buffer;
}

/**
 * Connects to the specified socket
 */
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



int send_message_int(int socket, int message, int message_size){
    void * buff = calloc(1,message_size+22);
    snprintf(buff,message_size+22,"PRE%dSUF",message);
    ssize_t len = send(socket, buff, message_size+22, 0);
        if (len < 0)
        {
              fprintf(stderr, "Error on sending message --> %s", strerror(errno));
              free(buff);
              return 1;
        }
    
    free(buff);
    return 0;
}


/**
 * Sends a text message to provided socket
 */
int send_message_char(int socket, char* message, int message_size){
    char * buff = calloc(1, message_size+LEN_OF_APPENDIX);
    int result = snprintf(buff,message_size+LEN_OF_APPENDIX,"PRE%sSUF",message);

    if(result < 0){
        fprintf(stderr, "Encoding error");
        free(buff);
        return 1;
    }
    fprintf(stdout,"SENDING %s\n",buff);
    ssize_t len = send(socket, buff, strlen(buff), 0);
    if (len < 0)
    {
            fprintf(stderr, "Error on sending message --> %s, %d", strerror(errno),len);
            free(buff);
            return 1;
    }
    if (len != strlen(buff)){
        fprintf(stderr,"Couldn't send the full message --> %s, %d",strerror(errno),len);
        free(buff);
        return 1;
    }

    free(buff);
    return 0;
}


/**
 * Send the specified audio frames to the provided socket 
 */
int send_frames(int socket, int* frames, int items_to_send, int size_of_frame, int frames_in_message){

    for(int i = 0; i< items_to_send-frames_in_message;){
        int offset = 0;
        char * buff = calloc(1,(size_of_frame+LEN_OF_APPENDIX)*frames_in_message);
        for(int j=0;j< frames_in_message;j++){
            offset += snprintf(buff+offset,size_of_frame+LEN_OF_APPENDIX,"PRE%dSUF",frames[i+j]);
            if( offset <0){
                fprintf(stderr,"Failed to prepare frame for sending\n");
                free(buff);
                return 1; 
            }
        }
        fprintf(stdout,"Sending frame %d out of %d, content %s\n",i,items_to_send,buff);
        int write_result = send(socket,buff,size_of_frame*frames_in_message,0);
        if (write_result < 0){
            fprintf(stderr,"Failed to send frame %d, write result: %d\n",i,write_result);
            free(buff);
            return 1;
        };
        i = i+frames_in_message;
        
        free(buff);
    }
    fprintf(stdout,"Sent all frames\n");
    return 0;
}

int disconnect (int socket)
{
    int status = close(socket);
    if (status == -1)
    {
        fprintf(stderr, "Failed to close connection for socket, id: %d: %s\n", socket, strerror(errno));
        exit(1);
    }
    printf("Closing connection with socket, id: %d\n", socket);

    return status;
}
