#ifndef PLAYERS_H
#define PLAYERS_H
#include "rifle_variables.h"
#include "inventory.h"
//typedef enum { false, true } bool;

typedef struct {
    unsigned char* ID;
    bool ALIVE;            //0 = dead, 1 = alive
    char HEALTH;
    char ARMOR;
    struct INVENTORY_ITEM* INVENTORY;
} PLAYER;

void set_player_info(PLAYER, unsigned char*);

#endif
