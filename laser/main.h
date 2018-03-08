#ifndef MAIN_H
#define MAIN_H
//used to control compilation flags
//IE DEBUG mode, disable I2C, etc.

#include <stdint.h>

struct samples_x3 {
    float x;
    float y;
    float z;
};

//#define DEBUG
#define CLIENT_ENABLE
#define IMU_ENABLE
#define WIRING_PINS

//#include "client.h"
//#include "dwm1000.h"
#include "lsm9ds1.h"

void alarmISR(int sig_num);

#endif //MAIN_H
