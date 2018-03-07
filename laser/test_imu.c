// Client side C/C++ program to demonstrate Socket programming
#include <math.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "main.h"

#define PI 3.14159

volatile unsigned char new_samples;

struct float_x3 ACCEL, GYRO, MAG;

void print_ACCEL(){
    float roll = atan2(ACCEL.y, ACCEL.z);
    float pitch = atan2(-ACCEL.x, sqrt(ACCEL.y * ACCEL.y + ACCEL.z * ACCEL.z));
    roll *= 180 /  PI;
    pitch *= 180 /  PI;
    printf("Pitch: %2.5f Roll: %2.5f\n", pitch, roll);
}

#define DECLINATION 11.32 //magnetic declination for Logan UT in degrees
void print_MAG(){
    //atan2(x, y) is the same as arctan(y/x)
    float th = atan2(MAG.x, MAG.y) * 180.0 / PI;
    
    //From arduino library
    float heading;
    if(MAG.y == 0)
        heading = (MAG.x < 0) ? PI : 0;
    else
        heading = atan2(MAG.x, MAG.y);
    
    heading -= DECLINATION * PI / 180.0;

    if (heading > PI) heading -= (2 * PI);
    else if (heading < -PI) heading += (2 * PI);
    else if (heading < 0) heading += 2 * PI;

    heading *= 180/PI;
    //End of Arduino Library definition

    if(th > 360)
        th -= 360;
    else if(th < 0)
        th += 360;
    
    printf("Th: %3.3f Heading: %3.3f\n", th, heading);

    //print out the correct orientation
    if(th < 22.5)
        printf("North\n");
    else if(th < 67.5)
        printf("North-East\n");
    else if(th < 112.5)
        printf("East\n");
    else if(th < 157.5)
        printf("South-East\n");
    else if(th < 202.5)
        printf("South\n");
    else if(th < 247.5)
        printf("South-West\n");
    else if(th < 292.5)
        printf("West\n");
    else if(th < 337.5)
        printf("North-West\n");
    else
        printf("North\n");    
}

void alarmISR(int sig_num){
    #ifdef DEBUG
        printf("ISR called for signal: %i\n", sig_num);
    #endif
    if(sig_num == SIGALRM){
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

        new_samples = 1;
    }
}

int main(){

    //pull readings from sensors
    if(init_IMU() > 0)
        return 1;
    calibrate_IMU();

    //define the ISR called for the SIGALRM signal
    signal(SIGALRM, alarmISR);  // jumps to alarmISR as the ISR
    ualarm(1000000, 1000000);     // trigger a SIGALRM signal every second        

    new_samples = 0;
    while(1){
        if(new_samples){
            new_samples = 0;
            print_ACCEL();
            print_MAG();
        }
    }

    return 0;
}
