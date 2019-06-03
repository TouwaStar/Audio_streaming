
#include "communication.c"
#include "../util/util.c"
#include "../third_party/sndfile.h"

#include "math.h"
#include "limits.h"
typedef enum{WINDOWS = 1, POSIX = 2} SYS;

#define QUEUE 512
#define PORT 8989
#define FRAMES_IN_MESSAGE 80
#define PATH_TO_SONGS "../audio_library"

#include <time.h>

int main(int argc, char **argv)
{

    
    int port;

    char* path_to_songs = NULL;
    char* song_to_stream = NULL;
    char* path_to_song;

    int socket;
    int peer_socket;

    SNDFILE *file;
    SF_INFO file_info;

    int items_to_alocate;
    int *frames = NULL;

    int size_of_frame;
    
    void cleanup(){
        fprintf(stderr,"Cleanup");

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
    if (SYSTEM == WINDOWS){
            WSADATA data;
            WSAStartup(MAKEWORD(2,2), &data);
        }

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
        path_to_songs = PATH_TO_SONGS;
    }else{
        path_to_songs = argv[2];
    }

    socket = create_socket(TCP);
    bind_port(socket, port);
    listen_to_socket(socket, QUEUE,TCP);
    peer_socket = accept_connection(socket);

    // Process the client untill he disconnects
    while(1){
        retrieve_message_dynamic(peer_socket);

        send_songs_list_to_client(peer_socket, path_to_songs);
        song_to_stream = get_song_to_stream_dynamic(peer_socket);
        path_to_song = create_path_dynamic(path_to_songs,song_to_stream);
        fprintf(stdout,"Path to song %s\n",path_to_song);
        free(song_to_stream);
        file = sf_open(path_to_song, SFM_READ, &file_info);

        items_to_alocate = file_info.frames * file_info.channels ;
        frames = calloc(items_to_alocate, sizeof(int));
        

        int number_of_frames = sf_readf_int(file, frames, file_info.frames);

        fprintf(stdout,"Sampling rate: %d\n",file_info.samplerate);
        fprintf(stdout,"Number of channels: %d\n",file_info.channels);
        fprintf(stdout,"Number of frames in file: %I64i\n", file_info.frames);
        fprintf(stdout,"Song length %I64d\n",file_info.frames/file_info.samplerate);
        fprintf(stdout,"Number of frames read: %d\n", number_of_frames);
        fprintf(stdout,"Size of single frame: %d\n", sizeof(frames[0]));

        // Calculate how big the frame needs to be after transforming from int to chars
        size_of_frame = floor(log10(llabs(LLONG_MAX)))+2;
        
        // Send properties to client
        send_audio_property(peer_socket, file_info.samplerate);
        send_audio_property(peer_socket, file_info.channels);
        // Send song length to client
        send_audio_property(peer_socket, file_info.frames/file_info.samplerate);
        // Send size of frame * number of frames as the size of buffer
        send_audio_property(peer_socket, size_of_frame*FRAMES_IN_MESSAGE);
        
        send_frames(peer_socket,frames,items_to_alocate,size_of_frame,FRAMES_IN_MESSAGE);
        send_message_char(peer_socket,"EOM",sizeof("EOM"));
    }

    if (SYSTEM == WINDOWS){
        WSACleanup();
    }
    return 0;
}