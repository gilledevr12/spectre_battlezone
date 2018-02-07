// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <unistd.h>

#include "main.h"

struct float_x3 ACCEL, GYRO, MAG;

int main(){

    #ifdef IMU_ENABLE
        #ifdef DEBUG
            printf("Begin IMU Initialization\n");
        #endif
        //pull readings from sensors
        if(init_IMU() > 0)
            return 1;
        calibrate_IMU();

    #else
        #ifdef DEBUG
            printf("Skipping the IMU Initialization\n");
        #endif
    #endif  //IMU_ENABLE

    #ifdef CLIENT_ENABLE
        #ifdef DEBUG
            printf("Begin client-communication initialization\n");
        #endif

        open_client_socket();
        close_client_socket();  //Don't leave open.. this should really be closed... 

        if(pull_DEVICE_MAC() > 0)
            return 1;

    #else
        printf("Client-comms not enabled. Messages will be sent to the console\n");
    #endif  //CLIENT_ENABLE

    int packet_count = 0, SLEEP_DELAY = 1000000;
    while(1){
        #ifdef IMU_ENABLE
            float* curr_samples = IMU_pull_samples();
            ACCEL.x = curr_samples[0];
            ACCEL.y = curr_samples[1];
            ACCEL.z = curr_samples[2];
            GYRO.x = curr_samples[3];
            GYRO.y = curr_samples[4];
            GYRO.z = curr_samples[5];
            MAG.x = curr_samples[6];
            MAG.y = curr_samples[7];
            MAG.z = curr_samples[8];
            
        #else
            //send test packet
            ACCEL.x = 1.00;
            ACCEL.y = 2.00;
            ACCEL.z = 3.00;
            GYRO.x = 4.00;
            GYRO.y = 5.00;
            GYRO.z = 6.00;
            MAG.x = 7.00;
            MAG.y = 8.00;
            MAG.z = 9.00;
        #endif

        open_client_socket();
        send_status(ACCEL, GYRO, MAG, 0, packet_count++);
        close_client_socket();
        usleep(SLEEP_DELAY);
        
        }

    return 0;
}
