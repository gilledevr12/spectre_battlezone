// Main function for Spectre Battlezone

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "play.h"
#include "setup.h"
#include "bool.h"

void print_help(){
    printf("Invalid syntax: requires one of two arguments:\n\t-p <map_file.sbm>  Indicates play mode using the declared .sbm file\n\t-s  Indicates setup mode, will start up the sequence for creating a map file");
}

char* get_map_name(){
    size_t buf_size = 32;
    size_t chars;
    char* map_name;
    map_name = (char*)malloc(buf_size * sizeof(char));
    printf("Which map file will you be using?\n");
    chars = getline(&map_name, &buf_size, stdin);
    return map_name;
}

int main(int argc, char *argv[]){
    int len;    // argument length
    int ch;     // character buffer
    char *str;  // string buffer
    str = malloc(4);
    char map_file[256];
    if(argc == 2 || argc == 3){
        for(int i=0; i<argc; i++){
            switch((int)argv[i][0]){
                case '-':
                    len = strlen(argv[i]);
                    for(int j=0; j<len; j++){
                        ch = (int)argv[i][j];
                        switch(ch){
                            case 's':
                                // Setup mode --map out the room
                                // then switch to play mode if desired
                                if(argc == 3){ print_help(); }
                                else{ create_map(); }
                            break;
                            case 'p':
                                // Play mode --playing the game
                                if(argc == 2){
                                    strcpy(map_file,get_map_name());
                                    play_loop(map_file);
                                }
                                else{
                                    if(j+1 > len){ print_help; }
                                    else{ strcpy(map_file, &argv[i][j+2]); }
                                    play_loop(map_file);
                                }
                            break;
                        }
                    }
                    break;
            }
        }
    }
    else{ print_help(); return 1;}
    return 0;
}

