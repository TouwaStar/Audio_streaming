
#include <sys/stat.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

struct wav_header
{
    char   id[4];            // pos 1-4 always RIFF
    int    file_size;        // pos 5-8 size of file (in bytes) - 8 bytes
    char   file_type[4];     // pos 9-12 should be WAVE
    char   format[4];        // pos 13-16 format chunk marker with trailing null "fmt "
    int    format_len;       // pos 17-20 exp 16
    short  pcm;              // pos 21-22 type of format 1 = pcm
    short  channels;         // pos 23-24 number of channels
    int    sampling_rate;    // pos 25-28 SR
    int    bytes_per_second; // pos 29-32
    short  bytes_by_capture; // pos 33-34
    short  bits_per_sample;  // pos 35-36
    char   data[4];          // pos 37-40 always "data", marks start of data section
    int    bytes_in_data;    // pos 41-44 size of data
};

int _verify_header(struct wav_header header){

    // if(strcmp(header.id, "RIFF")){
    //     fprintf(stderr, "Header id != RIFF\nHeader id: %s\n",header.id);
    //     return 1;
    // }
    if(strcmp(header.file_type, "WAVE")){
        fprintf(stderr, "File type != WAVE\nFile type: %s\n",header.file_type);
        return 1;
    }
    if(strcmp(header.format, "fmt ")){
        fprintf(stderr, "Format != fmt \n");
        return 1;
    }
    if(header.format_len != 16){
        fprintf(stderr, "Format len != 16\n");
        return 1;
    }
    if(header.pcm != 1){
        fprintf(stderr, "Pcm != 1\n");
        return 1;
    }
    if(header.channels != 1 && header.channels != 2){
        fprintf(stderr, "Channels != 1 && != 2\n");
        return 1;
    }
    if(strcmp(header.data, "data")){
        fprintf(stderr, "Data != data\n");
        return 1;
    }

    return 0;
}

struct wav_header read_wav(char* path){
    FILE *fp = fopen(path,"rb");
    struct wav_header header;
    if (fp){
        fread(&header,1,sizeof(struct wav_header), fp);

    }
    //char * data;
    //data = malloc(header.bytes_in_data);

    if(_verify_header(header)){
        fprintf(stderr,"Unreadeable format of wav header");
    }
    
    fclose(fp);
    return header;

}
