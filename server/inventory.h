#ifndef INVENTORY_H
#define INVENTORY_H
#include "rifle_variables.h"

typedef struct {
    WEAPON WEAPON_NAME;
    bool EQUIPPED;
    char AMMO_REMAINING;
    char MAX_AMMO;
} INVENTORY_ITEM;

//const struct INVENTORY_ITEM* no_items = NULL;
INVENTORY_ITEM* initial_inventory;



#endif
