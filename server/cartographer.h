#ifndef CARTOGRAPHER_H
#define CARTOGRAPHER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "array.h"

map_array build_array(FILE* map){
    map_array map_data;
    init_array(&map_data, 40);
    char line[20];
    while(fgets(line, sizeof(line), map)){
        coordinate temp = { 't', 0, 0 };
        //size_t line_len = strlen(line);
        //printf("size of this line is %i\n",line_len);
        int i = 0;
        for(char* t = strtok(line," "); t!=NULL; t = strtok(NULL," ")){
            if(i == 0){ temp.type = t[0]; }
            if(i == 1){ temp.x = atof(t); }
            if(i == 2){ temp.y = atof(t); }
            i++;
        }
        insert_array(&map_data,temp);
    }
    return map_data;
}

int min_x(const void* coor1, const void* coor2){
    printf("in min_x\n");
    coordinate* coora = (coordinate*)coor1;
    coordinate* coorb = (coordinate*)coor2;
    if(coora->x < coorb->x && coora->y > coorb->y)
        return 1;
    return -1;
}

int max_x(const void* coor1, const void* coor2){
    coordinate* coora = (coordinate*)coor1;
    coordinate* coorb = (coordinate*)coor2;
    if(coora->x > coorb->x && coora->y > coorb->y)
        return 1;
    return -1;
}

int min_y(const void* coor1, const void* coor2){
    coordinate* coora = (coordinate*)coor1;
    coordinate* coorb = (coordinate*)coor2;
    if(coora->x < coorb->x && coora->y < coorb->y)
        return 1;
    return -1;
}

int max_y(const void* coor1, const void* coor2){
    coordinate* coora = (coordinate*)coor1;
    coordinate* coorb = (coordinate*)coor2;
    if(coora->x > coorb->x && coora->y < coorb->y)
        return 1;
    return -1;
}

void sort_array(map_array *map_data, int method){
    if     (method == 1){ qsort(map_data, map_data->size, sizeof(map_data), min_x); }
    else if(method == 2){ qsort(map_data, map_data->size, sizeof(map_data), max_x); }
    else if(method == 3){ qsort(map_data, map_data->size, sizeof(map_data), min_y); }
    else if(method == 4){ qsort(map_data, map_data->size, sizeof(map_data), max_y); }
    else   { printf("invalid sort method\n"); }
}

void create_map(map_array map_data){
    int i = 0;
    for(i=0; i<map_data.size; i++){
        sort_array(&map_data, 1);
    }
}

#endif
