#ifndef MAIN_H
#define MAIN_H
//used to control compilation flags
//IE DEBUG mode, disable I2C, etc.

struct int_x3 {
    int x;
    int y;
    int z;
};

#define DEBUG 1
#define CLIENT_ENABLE
//#define SENSOR_ENABLE

#include "client.h"
#include "dwm1000.h"
#include "lsm9ds1.h"
#include "rifle_variables.h"



#endif //MAIN_H
