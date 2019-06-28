#include "communication.c"
#include "../util/util.c"

#if defined(_WIN32)
#include "../third_party/sndfile.h"
#else
#include "../third_party/sndfilel.h"
#endif // defined

#include "math.h"
#include "limits.h"



#define QUEUE 512
#define PORT 8989
#define FRAMES_IN_MESSAGE 80
#define PATH_TO_SONGS "./audio_library"

#define FATAL "FATAL"
#define SIZE_OF_FATAL 6

#include <time.h>



struct {
    char* SONG_LIST;
    char* SET_SONG;
    char* SAMPLING_RATE;
    char* CHANNELS;
    char* SONG_LENGTH;
    char* NUMBER_OF_FRAMES;
    char* BUFFER_SIZE;
    char* STREAM_SONG;
} Commands = {
"GIVE_SONG_LIST",
"SET_SONG_",
"GIVE_SAMPLING_RATE",
"GIVE_CHANNELS",
"GIVE_SONG_LENGTH",
"GIVE_NUMBER_OF_FRAMES",
"GIVE_BUFFER_SIZE",
"GIVE_STREAM_SONG"};







int main(int argc, char **argv)
{


    int port;

    char* path_to_songs = NULL;
    char* song_to_stream = NULL;
    char* path_to_song;

    int socket;
    int peer_socket;

    SNDFILE *file = NULL;
    SF_INFO file_info;

    int items_to_alocate;
    int *frames = NULL;

    int size_of_frame;

    void cleanup(){
        fprintf(stderr,"Cleanup\n");

        // Calling free on NULL pointer makes no action occur according to standard
        free(song_to_stream);
        free(path_to_song);
        free(frames);
        sf_close(file);

        close(peer_socket);
        close(socket);
    }
    atexit(cleanup);

    /* *
     *  Parsing of command line arguments
     *  Initializing System Specific features
     * */
    #if defined(_WIN32)
    WSADATA data;
    WSAStartup(MAKEWORD(2,2), &data);
    #endif
    if (argc<2)
    {
        // Use default
        port = PORT;
    }
    else
    {
        port = atoi(argv[1]);
        if (port == 0)
        {
            // Fallback to default if atoi fails
            port = PORT;
        }
    }if (argc<3)
    {
        // No path provided, attempting to use relative default
        DIR* dir = opendir(PATH_TO_SONGS);
        if (dir) {
        // Directory exists.
        closedir(dir);
        }else{
        // Relative default doesn't exist
        fprintf(stderr,"Can't find default audio library at path %s specfify path to audio library\n", PATH_TO_SONGS);
        fprintf(stderr,"Usage: server.exe [port] [audio_library_path]\n");
        exit(1);
        }
        path_to_songs = PATH_TO_SONGS;
    }else{
        path_to_songs = argv[2];
    }
    fprintf(stdout,"Server ip address %s\n",get_own_ip());
    socket = create_socket(TCP);
    bind_port(socket, port);
    listen_to_socket(socket, QUEUE,TCP);
    peer_socket = accept_connection(socket);

    // Process the client untill he disconnects
    char* request;
    while(1){


        free(request);
        request = retrieve_message_dynamic(peer_socket);

        if(strcmp(request,"CLOSED")==0){
            listen_to_socket(socket, QUEUE,TCP);
            peer_socket = accept_connection(socket);
        }

        // Is the client asking for song list
        if(strcmp(request,Commands.SONG_LIST) == 0){
            if(send_songs_list_to_client(peer_socket, path_to_songs)>0){
                send_message_char(peer_socket,"FATAL",SIZE_OF_FATAL);
            };

        }

        // Is the client telling us to set a song
        if(strlen(request)>strlen(Commands.SET_SONG)){
            char tempchar[strlen(Commands.SET_SONG)];
            for(int i = 0; i<strlen(Commands.SET_SONG);i++){
                tempchar[i] = request[i];
            }
            tempchar[strlen(Commands.SET_SONG)] = '\0';
            if(strcmp(tempchar,Commands.SET_SONG)==0){
                // todo

                int i = strlen(Commands.SET_SONG);
                int j = 0;
                char song[BUFF];
                while(request[i] != '\0'){
                    song[j]= request[i];
                    j++;
                    i++;
                }
                song[j]='\0';
                path_to_song = create_path_dynamic(path_to_songs,song);
                fprintf(stdout,"Path to song %s\n",path_to_song);
                file = sf_open(path_to_song, SFM_READ, &file_info);
            }
        }


        if (strcmp(request,Commands.SAMPLING_RATE) == 0){
            fprintf(stdout,"Client asked for sampling rate\n");
            if(file == NULL){
                fprintf(stderr,"No song has been selected\n");
                send_message_char(peer_socket,"NO_SONG_SELECTED",strlen("NO_SONG_SELECTED")+2);
            }
            else if(send_audio_property(peer_socket, file_info.samplerate) > 0){
                fprintf(stderr,"Sending sampling rate failed\n");
                send_message_char(peer_socket,"FATAL",SIZE_OF_FATAL);

            }

            fprintf(stdout,"Sampling rate: %d\n",file_info.samplerate);


        }

        if (strcmp(request,Commands.CHANNELS) == 0){
            if(file == NULL){
                fprintf(stderr,"No song has been selected\n");
                send_message_char(peer_socket,"NO_SONG_SELECTED",strlen("NO_SONG_SELECTED")+2);
            }
            else if(send_audio_property(peer_socket, file_info.channels) > 0){
                send_message_char(peer_socket,"FATAL",SIZE_OF_FATAL);
            }

            fprintf(stdout,"Number of channels: %d\n",file_info.channels);

        }


        if (strcmp(request,Commands.STREAM_SONG) == 0){
            if(file == NULL){
                fprintf(stderr,"No song has been selected\n");
                send_message_char(peer_socket,"NO_SONG_SELECTED",strlen("NO_SONG_SELECTED")+2);
            }

            fprintf(stdout,"Number of frames in file: %I64li\n", file_info.frames);

            items_to_alocate = file_info.frames * file_info.channels;
            frames = calloc(items_to_alocate, sizeof(int));
            int number_of_frames = sf_readf_int(file, frames, file_info.frames);
            fprintf(stdout,"Number of frames read: %d\n", number_of_frames);
            fprintf(stdout,"Size of single frame: %ld\n", sizeof(frames[0]));

            // Calculate how big the frame needs to be after transforming from int to chars
            size_of_frame = floor(log10(llabs(LLONG_MAX)))+2;
            send_audio_property(peer_socket, size_of_frame*FRAMES_IN_MESSAGE);

            send_frames(peer_socket,frames,items_to_alocate,size_of_frame,FRAMES_IN_MESSAGE);
            send_message_char(peer_socket,"EOM",sizeof("EOM"));\

            fprintf(stdout,"Number of channels: %d\n",file_info.channels);

        }

        if (strcmp(request,Commands.SONG_LENGTH) == 0){
            if(file == NULL){
                send_message_char(peer_socket,"NO_SONG_SELECTED",strlen("NO_SONG_SELECTED")+2);
            }
            else if(send_audio_property(peer_socket, file_info.frames/file_info.samplerate) > 0){
                send_message_char(peer_socket,"FATAL",SIZE_OF_FATAL);
            }

            fprintf(stdout,"Song length %I64li\n",file_info.frames/file_info.samplerate);
        }




    }
    #if defined(_WIN32)
    WSACleanup();
    #endif
    return 0;
}
