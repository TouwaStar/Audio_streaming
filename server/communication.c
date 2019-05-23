#include "./networking/networking.c"

void send_audio_property(int socket, int property){
    receive_data(socket);
    send_message_int(socket, property, sizeof(property));
    receive_data(socket);
}