// ascii gui for Spectre Battlezone

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "play.h"
#include "setup.h"
#include "layout.h"

typedef enum { false, true } bool;

void wait(int seconds){
    time_t start = time(0);
    while(time(0)-start<seconds);
}

int main(int argc, char *argv[]){
    bool setup_mode = false;
    bool play_mode = false;
    bool is_setup = false;
    bool done = false;
    int len;    // argument length
    int ch;     // character buffer
    char *str;  // string buffer
    str = malloc(4);
    for(int i=0; i<argc; i++){
        switch((int)argv[i][0]){
            case '-':
                len = strlen(argv[i]);
                for(int j=0; j<len; j++){
                    ch = (int)argv[i][j];
                    switch(ch){
                        case 's':
                            // Setup mode --map out the room
                            // This mode will map out the room, when finished, will write the map to a file
                            // then switch to play mode if desired
                            setup_mode = true;
                        break;
                        case 'p':
                            // Play mode --playing the game
                            // Will open a map file created from a previous setup then call the play function
                            play_mode = true;
                        break;
                    }
                }
                break;
        }
    }
    while(setup_mode){
        printf("\nTODO: Implement Setup Mode!\n");
        setup_mode = false;
    }
    while(play_mode){
        printf("__        __   _                            _        \n");
        printf("\\ \\      / /__| | ___ ___  _ __ ___   ___  | |_ ___  \n");
        printf(" \\ \\ /\\ / / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\ | __/ _ \\\n");
        printf("  \\ V  V /  __/ | (_| (_) | | | | | |  __/ | || (_) |\n");
        printf("   \\_/\\_/ \\___|_|\\___\\___/|_| |_| |_|\\___|  \\__\\___/ \n");
        printf("\n");                                             
        printf(" ____                  _            \n");
        printf("/ ___| _ __   ___  ___| |_ _ __ ___ \n");
        printf("\\___ \\| '_ \\ / _ \\/ __| __| '__/ _ \\ \n");
        printf(" ___) | |_) |  __/ (__| |_| | |  __/\n");
        printf("|____/| .__/ \\___|\\___|\\__|_|  \\___|\n");
        printf("      |_|                           \n");
        printf(" ____        _   _   _                          _ \n");
        printf("| __ )  __ _| |_| |_| | ___ _______  _ __   ___| |\n");
        printf("|  _ \\ / _` | __| __| |/ _ \\_  / _ \\| '_ \\ / _ \\ |\n");
        printf("| |_) | (_| | |_| |_| |  __// / (_) | | | |  __/_|\n");
        printf("|____/ \\__,_|\\__|\\__|_|\\___/___\\___/|_| |_|\\___(_)\n");
        done = true;
        if(done){
            play_mode = false;
        }
    }
    refresh_screen();
    while(1){
        wait(5);
        refresh_screen();
    }
    return 0;
}

                                                  
                                                  
