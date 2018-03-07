// This header will take in locationing data given during setup and will create a map file to use in gameplay
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "setup.h"
extern struct location_data loc_dat;

location_data current_location;

location_data get_location(){
    location_data current_location;
    current_location.x = 1;
    current_location.y = 2;
    current_location.z = 3;
    return current_location;
}

bool get_active_status(){
    return false;
}

void wait(int seconds){
    time_t start = time(0);
    while(time(0)-start<seconds);
}

void create_map(){
    bool active = false;
    char map_ftype[5];
    strcpy(map_ftype, ".sbm");
    size_t buf_size = 32;
    size_t chars;
    char* map_name;
    map_name = (char*)malloc(buf_size * sizeof(char));
    printf("What do you want to name the map?\n");
    chars = getline(&map_name, &buf_size, stdin);
    map_name = strtok(map_name, "\n");
    strcat(map_name,map_ftype);
    printf("The file we'll be using is %s\n",map_name);
    FILE* map;
    map = fopen(map_name, "w");
    /*while(active){
        get_active_status();
        current_location = get_location();
        fprintf(map, "%i\n", current_location.x);
        wait(1);
    }*/
}
