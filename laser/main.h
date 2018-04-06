#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

struct IMU_samples_x3 {
    int16_t x;
    int16_t y;
    int16_t z;
};

struct UWB_samples_x3{
    float A1;
    float A2;
    float A3;
};

#define TAG "TAG1"

#include "client.h"
#include "dwm1000.h"
#include "lsm9ds1.h"
#include <mosquitto.h>
#include "mqtt.h"

void alarmISR(int sig_num);

#endif //MAIN_H
