#include "../networking/networking.c"
#include "../util/util.c"

/**
 * Sends a generic audio property for example "Sampling" rate to the provided socket
 * Expects a message before and after sending indicating the will and readiness to receive
 * the property. 
 */
int send_audio_property(int socket, int property){
     if(send_message_int(socket, property, sizeof(property))>0){
        return 1;
     }
    return 0;
}

/**
 * Sends over the socket songs from the provided song list
 * up to a number provided in parameter
 */
int _send_available_songs(int socket, char** song_list, int number_of_songs){

    for(int i = 0; i < number_of_songs; i++){
        if(send_message_char(socket,song_list[i],strlen(song_list[i]))>0){
            return 1;
        }
    }
    if(send_message_char(socket,"SONG LIST EOM",strlen("SONG LIST EOM"))>0){
        return 1;
    }
    return 0;

}

/**
 * Send songs from specified directory to the specified socket
 */
int send_songs_list_to_client(int socket, char* path_to_songs){
    int number_of_files = 0;
    char **files = get_files_in_directory_dynamic(path_to_songs,&number_of_files);
    if(_send_available_songs(socket, files, number_of_files)>0){
        free(files);
        return 1;
    }
    free(files);
    return 0;
}

/**
 * Retrieves the name of the song to stream to the client from the specified socket.
 */
char* get_song_to_stream_dynamic(int socket){
    char* song = retrieve_message_dynamic(socket);
    return song;
}