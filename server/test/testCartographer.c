#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../cartographer.h"

void cart_test(map_array map_data){
    for(int i=0; i<map_data.size; i++){
        printf("%i: %s (%f,%f)\n");
    }
}

int main(){
    FILE* map = fopen("b.sbp","r");
    map_array map_data = build_array(map);
    cart_test(map_data);
    return 0;
}
