function makePlayer(spec){
    let that = {};

    let stats = {
        id: spec.id,
        alive: true,
        health: 100,
        armor: 0,
    }

    let weapons = [];
    weapons.push("pea_shooter");

    let position = {
        x: spec.position.x,
        y: spec.position.y
    }
    
    let direction = spec.direction;

    return that;
}

function add_weapon(player, weapon){
    player.weapons.push(weapon);
}

function set_direction(player, spec){
    player.direction = spec.direction;
}

function get_direction(player){
    return player.direction;
}

function set_position(player, spec){
    player.position.x = spec.position.x;
    player.position.y = spec.position.y;
}

function get_postion(player){
    return player.position;
}

function player_hit(distance, weapon){
    console.log("Player was hit!");
    let power = weapon.power;
    if(weapon === 'shotgun'){
        power = weapon.power - distance;
    }
    if(player.armor > 0){
        let remainder = 0;
        if(player.armor - power < 0){
            player.armor = 0;
            remainder = -1 * (player.armor - power); 
            player.health -= remainder;
        }
    }
    else{
        player.health -= power;
    }
    if(player.health <= 0){
        player.alive = false;
    }
}
        

/*#define WEAPON_COUNT    4

//WEAPON CLASS
#define PEA_SHOOTER     0
#define SHOTGUN         1
#define ASSAULT_RIFLE   2
#define SPECTRE_RIFLE   3

//RELOAD DELAY, FIRE DELAY
#define FAST            0
#define AVERAGE         1
#define SLOW            2

//FIRE WEIGHT (multiplier)
#define VARIES          0
#define LOW             0.5
#define MID             1
#define HIGH            2

typedef struct {
  char CLASS;
  char RELOAD_DELAY;
  char FIRE_DELAY;
  char FIRE_WEIGHT;
  char CLIP_SIZE;
} WEAPON;

#endif
#ifndef INVENTORY_H
#define INVENTORY_H
#include "rifle_variables.h"

typedef struct {
    WEAPON WEAPON_NAME;
    bool EQUIPPED;
    char AMMO_REMAINING;
    char MAX_AMMO;
} INVENTORY_ITEM;
*/

