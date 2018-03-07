// This header will take in locationing data given during setup and will create a map file to use in gameplay

#ifndef SETUP_H
#define SETUP_H
#include "bool.h"

typedef struct { int x; int y; int z; } location_data;

bool get_active_status();

void create_map();

#endif
