#ifndef MAPARRAY_H
#define MAPARRAY_H
#include <stdlib.h>

typedef struct{
    int x;
    int y;
    char type;
} coordinate;

typedef struct{
    coordinate *array;
    size_t used;
    size_t size;
} map_array;

void init_array(map_array *a, size_t initSize){
    a->array = (coordinate*)malloc(initSize * sizeof(coordinate));
    a->used = 0;
    a->size = initSize;
}

void insert_array(map_array *a, coordinate c){
    if(a->used == a->size){
        a->size *= 2;
        a->array = (coordinate*)realloc(a->array, a->size * sizeof(coordinate));
    }
    a->array[a->used++] = c;
}

void free_array(map_array *a){
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}

#endif
