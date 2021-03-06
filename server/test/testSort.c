#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../cartographer.h"

void cart_test(map_array map_data){
    for(int i=0; i<map_data.size; i++){
        printf("%i: %c (%.2f,%.2f)\n",i,map_data.array[i].type,map_data.array[i].x,map_data.array[i].y);
    }
}

int main(){
    FILE* map = fopen("b.sbp","r");
    map_array map_data = build_array(map);
    printf("\n-- Before Sorting --\n");
    cart_test(map_data);
    create_map(map_data);
    printf("\n-- After Sorting --\n");
    cart_test(map_data);
    return 0;
}
