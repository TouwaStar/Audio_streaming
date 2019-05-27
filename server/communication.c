#include "./networking/networking.c"

void send_audio_property(int socket, int property){
    receive_data(socket);
    send_message_int(socket, property, sizeof(property));
    receive_data(socket);
}

void send_available_songs(int socket, char** song_list, int number_of_songs){

    for(int i = 0; i < number_of_songs; i++){
        fprintf(stdout, "Sending %s",song_list[i]);
        send_message_char(socket,song_list[i],sizeof(song_list[i])*10);
    }
    send_message_char(socket,"SONG LIST EOM",sizeof("SONG LIST EOM"));



}