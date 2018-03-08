// Client side C/C++ program to demonstrate Socket programming
#include <math.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "main.h"

#define PI 3.14159

struct samples_x3 ACC, GYR, MAG;
#define ACC_LSB  0.001F
volatile unsigned char new_samples;

void print_ACCEL(){
	float pitch = asin( -ACC.x * ACC_LSB);
	float roll = asin(ACC.y * ACC_LSB / cos(pitch));
      
	float xh = MAG.x * cos(pitch) + MAG.z * sin(pitch);
	float yh = MAG.x * sin(roll) * sin(pitch) + MAG.y * cos(roll) -MAG.z * sin(roll) * cos(pitch);
//	float zh = -MAG.x * cos(roll) * sin(pitch) + MAG.y * sin(roll) + MAG.z * cos(roll) * cos(pitch);

	float heading = 180 * atan2(yh, xh)/PI;
	if (yh < 0)
		heading += 36;
	printf("Pitch: %3.3f \tRoll: %3.3f \txh: %3.3f \tyh: %3.3f\n", pitch, roll, xh, yh);
	printf("Tilt: %3.3f\n", heading);
}

#define DECLINATION 11.32 //magnetic declination for Logan UT in degrees
void print_MAG(){
	float heading = 180*atan2(MAG.y, MAG.x)/PI;  // assume pitch, roll are 0
	if(heading < 0)
		heading += 360;
	printf("Heading: %3.3f\n", heading);
}

void alarmISR(int sig_num){
    if(sig_num == SIGALRM){
        float* curr_samples = IMU_pull_samples();
        ACC.x = curr_samples[0];
        ACC.y = curr_samples[1];
        ACC.z = curr_samples[2];
        GYR.x = curr_samples[3];
        GYR.y = curr_samples[4];
        GYR.z = curr_samples[5];        
        MAG.x = curr_samples[6];
        MAG.y = curr_samples[7];
        MAG.z = curr_samples[8];

        new_samples = 1;
    	alarm(1);     // trigger a SIGALRM signal every second        
    }
}

int main(){

    //pull readings from sensors
    printf("Init the imu..");
    init_IMU();
    printf("done\n");
    
    //define the ISR called for the SIGALRM signal
    printf("Define the timer interrupt\n");
    signal(SIGALRM, alarmISR);  // jumps to alarmISR as the ISR
//    ualarm(1000000, 1000000);     // trigger a SIGALRM signal every second        
    alarm(1);     // trigger a SIGALRM signal every second        

    new_samples = 0;
    read_memory();
    while(1){
	    if(new_samples){
		    new_samples = 0;
        	    print_ACCEL();
	            print_MAG();
	    }
    }

    return 0;
}
