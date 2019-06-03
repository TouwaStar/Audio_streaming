// Mateusz Siłaczewski IZ06IO1 16084
#ifndef UTIL
#define UTIL
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

#define MAX_TEXT_SIZE 256
#include <dirent.h>

/**
 * Opens the file at specified path, returns the file descriptor.
 */
int open_file(char* path){
        
    int fd = open(path, O_RDONLY);
        if (fd == -1)
        {
                fprintf(stderr, "Error opening file --> %s", strerror(errno));

                exit(EXIT_FAILURE);
        }
        return fd;
}

/**
 * Returns a dynamically allocated pointer containing all the files found at the specified directory
 *  with the exception of files beggining with '.'. This function is also provided an int pointer to which
 *  it will write the number of files that were found in the directory.
 */
char** get_files_in_directory_dynamic( char* directory, int* number_of_files){
    DIR *d;
    struct dirent *dir;
    d = opendir(directory);
    
    char **files = calloc(100,100);
    if (d)
    {
        int i = 0;
        fprintf(stdout,"Retrieving available music files\n");
        while ((dir = readdir(d)) != NULL)
        {
            if(dir->d_name[0] == '.'){
                continue;
            }
            files[i] = malloc(MAX_TEXT_SIZE * sizeof(char));
            snprintf(files[i],MAX_TEXT_SIZE,dir->d_name);

            fprintf(stdout,"%s\n", dir->d_name);
            i++;
        }
        closedir(d);
        *number_of_files = i;
    }else{
        fprintf(stderr, "Unable to open audio directory");
        exit(1);
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

/**
 * Joins the two strings provided in order to create a path
 *  returns a char pointer to dynamically allocated memory.
 */
char* create_path_dynamic(char* string_a, char* string_b){
    char * path;
    if( (path = malloc(strlen(string_a)+strlen(string_b)+2)) != NULL){
        path[0] = '\0';   
        strcat(path,string_a);
        strcat(path,"/");
        strcat(path,string_b);
    } else {
        fprintf(stderr, "Creating path failed!\n");
    }
    return path;
}


#endif