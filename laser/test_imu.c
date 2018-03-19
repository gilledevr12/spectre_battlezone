// Client side C/C++ program to demonstrate Socket programming
#include <math.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "main.h"

#define PI 3.14159

//struct samples_x3 ACC, GYR, MAG;
struct int_samples_x3 ACC, GYR, MAG;
#define ACC_LSB  0.001F
volatile unsigned char new_samples;
volatile unsigned char CALIBRATION_COUNTER = 0;
const unsigned char RECAL_MAX_COUNT = 31;

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
	//printf("Mag: %3.3f %3.3f %3.3f\n", MAG.x, MAG.y, MAG.z);
	printf("Mag: %i %i %i\t", MAG.x, MAG.y, MAG.z);
	if(MAG.x < 0)
		if(MAG.y < 0)
			printf("Q3\t");
		else
			printf("Q2\t");
	else
		if(MAG.y < 0)
			printf("Q4\t");
		else
			printf("Q1\t");

	//printf("Mag: %i %i %i\n", MAG.x, MAG.y, MAG.z);
	float heading = atan2(MAG.y, MAG.x);  // assume pitch, roll are 0
	//if(heading < 0)
	//	heading += 360;
	printf("Heading: %3.4f\n", heading);
}

void alarmISR(int sig_num){
    if(sig_num == SIGALRM){
        //float* curr_samples = IMU_pull_samples();
        int16_t* curr_samples = IMU_pull_samples_int();
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
	CALIBRATION_COUNTER++;
	//alarm(1);     // trigger a SIGALRM signal every second        

    }
}

int main(){
    printf("main is launching now...\n");
    //pull readings from sensors
    printf("Init the imu..");
    init_IMU();
    printf("done\n");
    
    //define the ISR called for the SIGALRM signal
    printf("Define the timer interrupt\n");
    signal(SIGALRM, alarmISR);  // jumps to alarmISR as the ISR
    ualarm(250000, 250000);     // trigger a SIGALRM signal every 1/4 second        
//    alarm(1);     // trigger a SIGALRM signal every second        

    new_samples = 0;
    //read_memory();
    while(1){
	    if(new_samples){
		    new_samples = 0;
//        	    print_ACCEL();
	            print_MAG();
	    }

	    if(CALIBRATION_COUNTER >= RECAL_MAX_COUNT){
		    printf("Recalibrating...");
		    calibrate_IMU();
		    printf("done\n");
		    CALIBRATION_COUNTER = 1;
	    }
    }

    return 0;
}
