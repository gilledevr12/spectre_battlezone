#include <stdio.h>
#include "array.h"

void test(){
    map_array map;
    int x = 3, y = 5;
    char type = 'W';

    //x = y = 0;
    init_array(&map, 5); // Make an initial small array

    for(int i=0; i<40; i++){
        if(i < 20){ type = 'B'; }
        else{ type = 'W'; }
        x++; y++;
        coordinate temp;
        temp.x = x;
        temp.y = y;
        temp.type = type;
        insert_array(&map, temp);
    }

    for(int i=0; i<map.size; i++){
        printf("%c: %i,%i\n",map.array[i].type,map.array[i].x,map.array[i].y);
    }

    free_array(&map);
    if(map.size == 0 && map.array == NULL){
        printf("Map is deallocated!\n");
    }
}

int main(){
    test();
    return 0;
}
