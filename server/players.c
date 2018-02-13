/* This file will include all of the possible players
 * If a player is placed in the game, variables for that player will
 * be initialized to non-zero values. Otherwise, all of the player
 * info will remain 0 indicating that player is not being used.
 */
#include "players.h"
#include "rifle_variables.h"
#include "guns.h"
typedef enum { false, true } bool;

unsigned char total_player_cnt = 0;

INVENTORY_ITEM* no_items;
initial_inventory.pea_shooter    = false;
initial_inventory.shotgun        = false;
initial_inventory.assault_rifle  = false;
initial_inventory.spectre_rifle  = false;
initial_inventory.spectre_status = false;
initial_inventory.quad_dmg       = false;


INVENTORY_ITEM* initial_inventory;
initial_inventory.pea_shooter    = true;
initial_inventory.shotgun        = false;
initial_inventory.assault_rifle  = false;
initial_inventory.spectre_rifle  = false;
initial_inventory.spectre_status = false;
initial_inventory.quad_dmg       = false;

struct PLAYER p1 = {0, 0, 0, 0, no_items};
struct PLAYER p2 = {0, 0, 0, 0, no_items};
struct PLAYER p3 = {0, 0, 0, 0, no_items};
struct PLAYER p4 = {0, 0, 0, 0, no_items};
typedef enum { p1, p2, p3, p4 } players;            // This could easily be extended to more than 4

set_player_id(unsigned char* MAC){
    // TODO talk to Taylor about making this happen
    return MAC;
}

set_player_info(unsigned char* id){
   current_player players = player_init_num;
   current_player.ID = id;
   current_player.ALIVE = true;
   current_player.HEALTH = 100;
   current_player.ARMOR = 0;
   current_player.INVENTORY = intial_inventory;
   total_player_cnt++;
}

#endif
