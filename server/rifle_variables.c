#include "rifle_variables.h"
#include "inventory.h"

//WEAPON name         = {CLASS, RELOAD_DELAY, FIRE_DELAY, FIRE_WEIGHT, CLIP_SIZE}
WEAPON pea_shooter    = {PEA_SHOOTER, AVERAGE, AVERAGE, MID, 15};
WEAPON shotgun        = {SHOTGUN, SLOW, AVERAGE, VARIES, 6};
WEAPON assault_rifle  = {ASSAULT_RIFLE, SLOW, FAST, LOW, 30};
WEAPON spectre_rifle  = {SPECTRE_RIFLE, FAST, AVERAGE, HIGH, 30};

INVENTORY_ITEM INVENTORY[WEAPON_COUNT];
