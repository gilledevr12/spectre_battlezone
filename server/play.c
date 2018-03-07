#include <time.h>
#include <pthread.h>
#include <string.h>
#include "play.h"
#include "layout.h"
#include "server.h"
#include "bool.h"

pthread_t server_thread, gameplay_display_thread;

void wait(float seconds){
    time_t start = time(0);
    while(time(0)-start<seconds);
}

bool valid_map_file(char* map_file){
    const char f_type[4] = ".bmp";
    char* valid;
    valid = strstr(map_file, f_type);
    if(valid != NULL){ return true; }
    return false;
}

void play_loop(char* map_file){
    if(valid_map_file(map_file)){
        pthread_create(&server_thread,NULL,server_loop,NULL);
        wait(0.01);    // Correcting an issue with the "Server launch" print statement
        printf("Using map file %s\n",map_file);
        refreshScreen();
        while(1){
            wait(5);
            refreshScreen();
        }
    }
    else{ printf("The supplied file was not a valid .bmp file\n"); exit(1);}
}
