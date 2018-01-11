// This is the initial layout of the gui. Will be altered as the game progresses and during setup
#ifndef LAYOUT_H
#define LAYOUT_H

void printMap(){
    int x = 30;
    int y = 30;
    for(int i=0; i<x+1; i++){
        printf("#");
    }
    printf("\n");
    for(int j=0; j<y; j++){
        printf("#");
        for(int k=0; k<x-1; k++){
            printf(" ");
        }
        printf("#\n");
    }
    for(int i=0; i<x+1; i++){
        printf("#");
    }
    printf("\n");
}

#endif
