#ifndef CARTOGRAPHER_H
#define CARTOGRAPHER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "array.h"

map_array build_array(FILE* map){
    map_array map_data;
    init_array(&map_data, 5);
    char line[10];
    while(fgets(line, sizeof(line), map)){
        coordinate temp;
        for(char* t = strtok(line," "); t!=NULL; t = strtok(NULL," ")){
            //char* t = strtok(line," ");
            printf("%s\n",t);
            //if(i == 0){ temp.type = t[0]; printf("%s ",t); }
            //if(i == 1){ temp.x = 2.72; printf("%.02f ",atof(t)); }
            //if(i == 2){ temp.y = 3.14; printf("%.02f\n",atof(t)); }
        }
        insert_array(&map_data,temp);
        //printf("%s",line);
    }
    return map_data;
}

#endif
