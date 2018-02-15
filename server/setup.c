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
    char* map_name;
    printf("What do you want to name the map?\n");
    scanf("%s",map_name);
    strcat(map_name,".map");
    FILE* map;
    map = fopen(map_name, "w");
    while(active){
        get_active_status();
        current_location = get_location();
        fprintf(map, "%i\n", current_location.x);
        wait(1);
    }
}
