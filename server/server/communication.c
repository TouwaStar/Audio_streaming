#include "../networking/networking.c"
#include "../util/util.c"

/**
 * Sends a generic audio property for example "Sampling" rate to the provided socket
 * Expects a message before and after sending indicating the will and readiness to receive
 * the property. 
 */
void send_audio_property(int socket, int property){
    receive_data(socket);
    send_message_int(socket, property, sizeof(property));
    receive_data(socket);
}

/**
 * Sends over the socket songs from the provided song list
 *  up to a number provided in parameter
 */
void _send_available_songs(int socket, char** song_list, int number_of_songs){

    for(int i = 0; i < number_of_songs; i++){
        send_message_char(socket,song_list[i],sizeof(song_list[i])*10);
    }
    send_message_char(socket,"SONG LIST EOM",sizeof("SONG LIST EOM"));

}

/**
 * Send songs from specified directory to the specified socket
 */
void send_songs_list_to_client(int socket, char* path_to_songs){
    int number_of_files = 0;
    char **files = get_files_in_directory_dynamic(path_to_songs,&number_of_files);
    _send_available_songs(socket, files, number_of_files);
    free(files);
}


char* get_song_to_stream_dynamic(int socket){
    char* song = retrieve_message_dynamic(socket);
    return song;
}