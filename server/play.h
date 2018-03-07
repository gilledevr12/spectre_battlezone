// This header will be called for a game. It will take a map file as input, update the display etc.

#ifndef PLAY_H 
#define PLAY_H 
#include "server.h"
#include "bool.h"

void wait(float);
void play_loop(char*);
bool valid_map_file(char*);

#endif
