#ifndef RIFLE_VARIABLES_H
#define RIFLE_VARIABLES_H

typedef enum { false, true } bool;

/* Define the weapons used in the game:
 *  PEA_SHOOTER:  typically the default weapon. average reload_delay, average fire_delay, average fire_weight
 *  SHOTGUN:  shotgun... slow reload_delay, average fire_delay, fire_weight varies with distance from target
 *  ASSAULT_RIFLE:  long reload_delay, short fire_delay, large clip size, low fire_weight
 *  SPECTRE_RIFLE:  short reload_delay, short fire_delay, large clip size, high fire_weight
 */

#define WEAPON_COUNT    4

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

struct WEAPON {
  char CLASS;
  char RELOAD_DELAY;
  char FIRE_DELAY;
  char FIRE_WEIGHT;
  char CLIP_SIZE;
};

#endif
