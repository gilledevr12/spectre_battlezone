#ifndef PLAYER_VARIABLES_H
#define PLAYER_VARIABLES_H
#include "rifle_variables.h"
typedef enum { false, true } bool;

struct PLAYER {
    char ID;
    char STATUS;
    char HEALTH;
    char ARMOR;
    INVENTORY_ITEM* INVENTORY;
}

#endif
