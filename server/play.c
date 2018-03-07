#include <time.h>
#include <pthread.h>
#include <string.h>
#include "play.h"
#include "layout.h"
#include "server.h"
#include "bool.h"

pthread_t server_thread, gameplay_display_thread;

typedef struct {
    int rows;
    int cols;
} file_size;

void wait(float seconds){
    time_t start = time(0);
    while(time(0)-start<seconds);
}

bool valid_map_file(char* map_file){
    const char f_type[4] = ".sbm";
    char* valid;
    valid = strstr(map_file, f_type);
    if(valid != NULL){ return true; }
    return false;
}

file_size get_map_size(FILE *map_file){
    file_size map_size = {0,0};
    char ch;
    int num_rows = 0, num_cols = 0, max_cols = 0;
    while(!feof(map_file)){
        ch = fgetc(map_file);
        if(ch != '\n'){ num_cols++; }
        else          { num_rows++; num_cols = 0; }
        if(num_cols > max_cols){ max_cols = num_cols; }
    }
    map_size.cols = max_cols;
    map_size.rows = num_rows;
    return map_size;
}

void play_loop(char* map_file){
    if(valid_map_file(map_file)){
        file_size map_size = {0,0};
        FILE *map;
        map = fopen(map_file,"r");
        if(map == NULL){
            printf("Map file not found, check the spelling of the input file or run the program in setup mode to create a map file.\n");
            exit(1);
        } else { 
            map_size = get_map_size(map);
        }
        pthread_create(&server_thread,NULL,server_loop,NULL);
        wait(0.01);    // Correcting an issue with the "Server launch" print statement
        printf("Using map file %s\n",map_file);
        printf("This map has %i columns and %i rows\n",map_size.cols,map_size.rows);
        refreshScreen();
        while(1){
            wait(5);
            refreshScreen();
        }
    }
    else{ printf("The supplied file was not a valid .bmp file\n"); exit(1);}
}
