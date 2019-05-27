// Mateusz Siłaczewski IZ06IO1 16084

#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef _WIN32
    #define SYSTEM WINDOWS
#else 
    #define SYSTEM POSIX
#endif

#define MAX_TEXT_SIZE 64
#include <dirent.h>

int open_file(char* path){
        
    int fd = open(path, O_RDONLY);
        if (fd == -1)
        {
                fprintf(stderr, "Error opening file --> %s", strerror(errno));

                exit(EXIT_FAILURE);
        }
        return fd;
}

char** get_files_in_directory( char* directory, int* number_of_files){
    DIR *d;
    struct dirent *dir;
    d = opendir(directory);
    
    char **files = calloc(100,MAX_TEXT_SIZE*sizeof(char*));
    if (d)
    {
        int i = 0;
        fprintf(stdout,"Retrieving available music files\n");
        while ((dir = readdir(d)) != NULL)
        {
            files[i] = malloc((MAX_TEXT_SIZE));
            snprintf(files[i],MAX_TEXT_SIZE,dir->d_name);
            
            fprintf(stdout,"%s\n", dir->d_name);
            i++;
        }
        closedir(d);
        *number_of_files = i;
    }
    return files;
}


void get_file_stat(int fd, struct stat *file_stat){
    /* Get file stats */
        if (fstat(fd, file_stat) < 0)
        {
                fprintf(stderr, "Error fstat --> %s", strerror(errno));

                exit(EXIT_FAILURE);
        }
}


