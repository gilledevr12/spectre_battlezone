// ascii gui for Spectre Battlezone

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "play.h"
#include "setup.h"
#include "layout.h"

typedef enum { false, true } bool;

int main(int argc, char *argv[]){
    bool setupMode = false;
    bool playMode = false;
    bool isSetup = false;
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
                            setupMode = true;
                        break;
                        case 'p':
                            // Play mode --playing the game
                            // Will open a map file created from a previous setup then call the play function
                            playMode = true;
                        break;
                    }
                }
                break;
        }
    }
    printMap();
    return 0;
}

