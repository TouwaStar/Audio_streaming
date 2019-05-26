#include "./networking/networking.c"

#ifdef _WIN32
    #define SYSTEM WINDOWS
#else 
    #define SYSTEM POSIX
#endif


#include <dirent.h>

char* get_files_in_directory(char* directory){
    DIR *d;
    struct dirent *dir;
    d = opendir(directory);
    

    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            printf("%s\n", dir->d_name);
        }
        closedir(d);
    }
    return "";
}

void send_audio_property(int socket, int property){
    receive_data(socket);
    send_message_int(socket, property, sizeof(property));
    receive_data(socket);
}