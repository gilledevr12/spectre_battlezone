#ifndef INVENTORY_H
#define INVENTORY_H
#include "rifle_variables.h"

struct INVENTORY_ITEM {
    struct WEAPON WEAPON_NAME;
    bool EQUIPPED;
    char AMMO_REMAINING;
    char MAX_AMMO;
};

//const struct INVENTORY_ITEM* no_items = NULL;
struct INVENTORY_ITEM* initial_inventory;



#endif
