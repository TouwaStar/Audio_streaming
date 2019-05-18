#include "./networking/networking.c"
#include "./util/util.c"
#include "./util/wav_parser.c"
#include "./third_party/sndfile.h"

typedef enum{WINDOWS = 1, POSIX = 2} SYS;

#ifdef _WIN32
    #define SYSTEM WINDOWS
#else 
    #define SYSTEM POSIX
#endif

#define QUEUE 256
#define PORT 6999

#include <time.h>

char * TEMP = "C:\\Users\\Mateusz\\projekt_programowanie\\server\\audio_library\\Yamaha-V50-Rock-Beat-120bpm.wav";

int main(int argc, char **argv)
{
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

    SNDFILE *file;
    SF_INFO file_info;

    file = sf_open(TEMP, SFM_READ, &file_info);

    fprintf(stdout,"Sampling rate: %d\n",file_info.samplerate);
    fprintf(stdout, "Number of channels: %d\n",file_info.channels);
    int items_to_alocate = file_info.frames * file_info.channels ;
    int *frames = calloc(items_to_alocate, sizeof(int));
    

    fprintf(stdout,"Number of frames in file: %I64i\n", file_info.frames);

    fprintf(stdout,"Number of frames read: %I64i\n",sf_readf_int(file, frames, file_info.frames));
    fprintf(stdout,"Size of single frame: %d\n", sizeof(frames[0]));

    
    int socket = create_socket(TCP);
    bind_port(socket, port);
    

    listen_to_socket(socket, QUEUE,TCP);

    int peer_socket = accept_connection(socket);

    send_frames(peer_socket,frames,items_to_alocate);
    
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