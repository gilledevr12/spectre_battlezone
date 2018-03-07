// This is the initial layout of the gui. Will be altered as the game progresses and during setup
#ifndef LAYOUT_H
#define LAYOUT_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void printTopBar(){
    printf("\nName           This is              K/D\n");
    printf("Evan           a message            7/2\n");
}

void printMap(){
    int x = 56;
    int y = 24;
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

void printBottomBar(){
    int coor_x = 1, coor_y = 5;
    char *heading = (char*)malloc(10);
    char *tilt    = (char*)malloc(10);
    strcpy(heading,"NE");
    strcpy(tilt,"up");
    //printf("Health    Armor        Weapon\n");
    //printf("100       25          Shotgun\n");
    printf("Heading          Tilt            Coordinate\n");
    printf("%s               %s              (%i,%i)\n",heading,tilt,coor_x,coor_y);

}

void refreshScreen(){
    printTopBar();
    printMap();
    printBottomBar();
}
#endif
