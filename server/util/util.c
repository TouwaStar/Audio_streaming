// Mateusz Siłaczewski IZ06IO1 16084

#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

int open_file(char* path){
        
    int fd = open(path, O_RDONLY);
        if (fd == -1)
        {
                fprintf(stderr, "Error opening file --> %s", strerror(errno));

                exit(EXIT_FAILURE);
        }
        return fd;
}

void get_file_stat(int fd, struct stat *file_stat){
    /* Get file stats */
        if (fstat(fd, file_stat) < 0)
        {
                fprintf(stderr, "Error fstat --> %s", strerror(errno));

                exit(EXIT_FAILURE);
        }
}


