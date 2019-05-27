
#include "communication.c"
#include "./util/util.c"
#include "./util/wav_parser.c"
#include "./third_party/sndfile.h"

typedef enum{WINDOWS = 1, POSIX = 2} SYS;

#define QUEUE 256
#define PORT 6999
#define FRAMES_IN_MESSAGE 240

#include <time.h>

char * TEMP = "C:\\Users\\Mateusz\\projekt_programowanie\\server\\audio_library\\05 Track 5.wav";
//char * TEMP = "C:\\Users\\Mateusz\\projekt_programowanie\\server\\audio_library\\Yamaha-V50-Rock-Beat-120bpm.wav";
int main(int argc, char **argv)
{
    /* *
     *  Parsing of command line arguments
     *  Initializing System Specific features
     * */
    if (SYSTEM == WINDOWS){
            WSADATA data;
            WSAStartup(MAKEWORD(2,2), &data);
        }

    int port;
    if (argc<2)
    {
        //use default
        port = PORT;
    }
    else
    {
        port = atoi(argv[1]);
        if (port == 0)
        {
            // fallback to default if atoi fails
            port = PORT;
        }
    }

    int socket = create_socket(TCP);
    bind_port(socket, port);
    

    listen_to_socket(socket, QUEUE,TCP);

    int peer_socket = accept_connection(socket);

    int number_of_files = 0;
    char **files = get_files_in_directory("C:\\Users\\Mateusz\\projekt_programowanie\\server\\audio_library",&number_of_files);
    send_available_songs(socket, files, number_of_files);
    free(files);

    SNDFILE *file;
    SF_INFO file_info;

    file = sf_open(TEMP, SFM_READ, &file_info);

    int items_to_alocate = file_info.frames * file_info.channels ;
    int *frames = calloc(items_to_alocate, sizeof(int));
    



    fprintf(stdout,"Sampling rate: %d\n",file_info.samplerate);
    fprintf(stdout,"Number of channels: %d\n",file_info.channels);
    fprintf(stdout,"Number of frames in file: %I64i\n", file_info.frames);
    fprintf(stdout,"Number of frames read: %I64i\n",sf_readf_int(file, frames, file_info.frames));
    fprintf(stdout,"Size of single frame: %d\n", sizeof(frames[0]));
    //  for( int i = 5000; i <5100;i++){
    //      fprintf(stdout,"Example frame %f\n",frames[i]);
    //  }
    
    

    int size_of_frame = sizeof(int)+20; // HOW BIG THIS THING NEEDS TO BEE TODO
    
    // Send properties to client
    send_audio_property(peer_socket, file_info.samplerate);
    send_audio_property(peer_socket, file_info.channels);

    // Send size of frame * number of frames as the size of buffer
    send_audio_property(peer_socket, size_of_frame*FRAMES_IN_MESSAGE);
    
    send_frames(peer_socket,frames,items_to_alocate,size_of_frame,240);
    send_message_char(peer_socket,"EOM",sizeof("EOM"));
    



    Sleep(5000);
    free(frames);
    sf_close(file);

    close(peer_socket);
    close(socket);
    


    if (SYSTEM == WINDOWS){
        WSACleanup();
    }
    return 0;
}