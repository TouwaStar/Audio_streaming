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
#define PORT 4200


char * TEMP = "C:\\Users\\Mateusz\\projekt_programowanie\\server\\audio_library\\Yamaha-V50-Rock-Beat-120bpm.wav";

int main(int argc, char **argv)
{

    SNDFILE *file;
    SF_INFO file_info;

    file = sf_open(TEMP, SFM_READ, &file_info);

    
    fprintf(stdout,"%d\n",file_info.samplerate);

    int *frames = malloc(file_info.frames * file_info.channels * sizeof(int));

    fprintf(stdout,"Number of frames: %I64i\n", file_info.frames);

    fprintf(stdout,"%I64i\n",sf_readf_int(file, frames, file_info.frames));

    fprintf(stdout,"Value of read frames:  %d\n", frames[6] );

    free(frames);
    sf_close(file);

    // if (SYSTEM == WINDOWS){
    //     WSADATA data;
    //     WSAStartup(MAKEWORD(2,2), &data);
    // }

    // int port;
    // if (argc<2)
    // {
    //     //use default
    //     port = PORT;
    // }
    // else
    // {
    //     port = atoi(argv[1]);
    //     if (port == 0)
    //     {
    //         // fallback to default if atoi fails
    //         port = PORT;
    //     }
    // }
    
    // int socket = create_socket(TCP);
    // bind_port(socket, port);
    

    // listen_to_socket(socket, QUEUE,TCP);

    // int fd = open_file("");
    
    // fprintf(stdout,"File opened");
    // struct stat file_stat;
    // get_file_stat(fd, &file_stat);
    // fprintf(stdout,"File stat read");
    // char file_size[256];
    // sprintf(file_size, "%d", file_stat.st_size);
    // fprintf(stdout,"File size calculated");
    
    // int peer_socket = accept_connection(socket);

    // send_file_size(socket, TEMP);



    // close(peer_socket);
    // close(socket);
    


    // if (SYSTEM == WINDOWS){
    //     WSACleanup();
    // }
    // return 0;
}