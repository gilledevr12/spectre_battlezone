#ifndef PLAYERS_H
#define PLAYERS_H
#include "rifle_variables.h"
typedef enum { false, true } bool;

struct INVENTORY_ITEM {
    bool pea_shooter;
    bool shotgun;
    bool assault_rifle;
    bool spectre_rifle;
    bool spectre_status;
    bool quad_dmg;
}

struct PLAYER {
    unsigned char* ID;
    bool ALIVE;            //0 = dead, 1 = alive
    char HEALTH;
    char ARMOR;
    INVENTORY_ITEM* INVENTORY;
}

set_player_id(unsigned char*);
set_player_info(unsigned char*);

#endif
