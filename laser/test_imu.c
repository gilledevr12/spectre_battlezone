// Client side C/C++ program to demonstrate Socket programming
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "lsm9ds1.h"

struct samples_x3 {
    float x;
    float y;
    float z;
};

struct int_samples_x3 {
    int16_t x;
    int16_t y;
    int16_t z;
};
#define PI 3.14159

//struct samples_x3 ACC, GYR, MAG;
struct int_samples_x3 ACC, GYR, MAG;
#define ACC_LSB  0.001F
volatile unsigned char new_samples;
volatile unsigned char CALIBRATION_COUNTER = 0;
const unsigned char RECAL_MAX_COUNT = 31;
#define ACC_FLAT	1500
#define ACC_UP		15500
#define ACC_DOWN 	-16000
#define MAG_NORTH	130.0

void print_ACCEL(){
	//each axis will return a value [-16500, +16500]
	if((ACC.x < ACC_FLAT) && (ACC.x > -ACC_FLAT) && (ACC.y < ACC_FLAT) && (ACC.y > -ACC_FLAT)) 
		printf("Flat enough to shoot!\n");
	else if(ACC.x < ACC_DOWN)
		printf("Straight down\n");
	else if(ACC.x > ACC_UP)
		printf("Straight up\n");
	else
		printf("Pointed in an invalid direction: %i %i %i\n", ACC.x, ACC.y, ACC.z);
	return;

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
	printf("Main Mag: %i %i %i\t", MAG.x, MAG.y, MAG.z);
	float heading = atan2(MAG.y, MAG.x);  // assume pitch, roll are 0
	heading *= 180 / PI;
	//if -180 < theta < -115, theta += 360
	if(heading < -MAG_NORTH)
		heading += 360.0 + MAG_NORTH;
	else
		heading += MAG_NORTH;
	//heading = heading + 180 - DECLINATION;
		
	printf("Heading: %3.4f\n", heading);
}

void alarmISR(int sig_num){
    if(sig_num == SIGALRM){
        //float* curr_samples = IMU_pull_samples();
        int16_t* curr_samples = IMU_pull_samples_int();
        ACC.x = curr_samples[0];
        ACC.y = curr_samples[1];
        ACC.z = curr_samples[2];
        //GYR.x = curr_samples[3];
        //GYR.y = curr_samples[4];
        //GYR.z = curr_samples[5];        
        MAG.x = curr_samples[3];
        MAG.y = curr_samples[4];
        MAG.z = curr_samples[5];
        
	new_samples = 1;
	CALIBRATION_COUNTER++;
	//alarm(1);     // trigger a SIGALRM signal every second        

    }
}

int main(){
    //printf("main is launching now...\n");
    //pull readings from sensors
    printf("Init the imu..");
    init_IMU();
    printf("done\n");
    
    //define the ISR called for the SIGALRM signal
    //printf("Define the timer interrupt\n");
    signal(SIGALRM, alarmISR);  // jumps to alarmISR as the ISR
    ualarm(250000, 250000);     // trigger a SIGALRM signal every 1/4 second        
//    alarm(1);     // trigger a SIGALRM signal every second        

    new_samples = 0;
    //read_memory();
    printf("x y\n");
    while(1){
	    if(new_samples){
		    new_samples = 0;
//		    printf("%i %i %i\n", ACC.x, ACC.y, ACC.z);
//        	    print_ACCEL();
	            print_MAG();
//		    printf("%i %i %i\n", MAG.x, MAG.y, MAG.z);
	    }

	    if(CALIBRATION_COUNTER >= RECAL_MAX_COUNT){
		    //printf("Recalibrating...");
		    //calibrate_IMU();
		    //printf("done\n");
		    CALIBRATION_COUNTER = 1;
	    }
    }

    return 0;
}
