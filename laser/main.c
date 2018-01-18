// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <unistd.h>

#include "main.h"


struct int_x3 ACCEL, MAGNETOM;

int main(){

    #ifdef SENSOR_ENABLE
        if(DEBUG)
            printf("Begin I2C methods\n");

        //pull readings from sensors
        char packet_buffer[100];

        int I2C_PORT = open_I2C_port("/dev/i2c-1");
        config_LSM303(I2C_PORT);
        ACCEL = get_accel(I2C_PORT);
        MAGNETOM = get_magnetom(I2C_PORT);
    #else
    if(DEBUG)
        printf("Skipping the I2C methods\n");
    #endif

    #ifdef CLIENT_ENABLE
        if(DEBUG)
            printf("Begin client methods\n");

        int ret = pull_DEVICE_MAC();
        if(!ret)
            return 1;

        //send test packet
        ACCEL.x = 100;
        ACCEL.y = 200;
        ACCEL.z = 300;
        MAGNETOM.x = 400;
        MAGNETOM.y = 500;
        MAGNETOM.z = 600;

        int count = 0, SLEEP_DELAY = 250;

        //send_status(ACCEL, MAGNETOM, HOTS_FIRED, FIRE_WEIGHT, 6, 1);
        while(1){
            open_client_socket();
            send_status(ACCEL, MAGNETOM, 0, count++);
            close_client_socket();
            usleep(SLEEP_DELAY);
        }

    #endif

    return 0;
}
