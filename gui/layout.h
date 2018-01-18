// This is the initial layout of the gui. Will be altered as the game progresses and during setup
#ifndef LAYOUT_H
#define LAYOUT_H

void printTopBar(){
    printf("\nName      This is         K/D\n");
    printf("Evan      a message       7/2\n");
}

void printMap(){
    int x = 28;
    int y = 12;
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
    printf("Health    Armor        Weapon\n");
    printf("100       25          Shotgun\n");
}

void refreshScreen(){
    printTopBar();
    printMap();
    printBottomBar();
}
#endif