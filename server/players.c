/* This file will include all of the possible players
 * If a player is placed in the game, variables for that player will
 * be initialized to non-zero values. Otherwise, all of the player
 * info will remain 0 indicating that player is not being used.
 */
#include "players.h"
#include "rifle_variables.h"
#include "guns.h"
#include "inventory.h"
typedef enum { false, true } bool;

unsigned char player_init_num = 0;

struct PLAYER p1 = {0, 0, 0, no_items};
struct PLAYER p2 = {0, 0, 0, no_items};
struct PLAYER p3 = {0, 0, 0, no_items};
struct PLAYER p4 = {0, 0, 0, no_items};
typedef enum { p1, p2, p3, p4 } players;

set_player_info(PLAYER current_player, unsigned char* id){
   current_player.ID = id;
   current_player.ALIVE = true;
   current_player.HEALTH = 100;
   current_player.ARMOR = 0;
   current_player.INVENTORY = intial_inventory;
}
