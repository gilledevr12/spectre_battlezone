/* main.c
 *
 * This file will compile into the main laser-brains method.
 * The underlying flow of the laser-brains is broken into several sequential
 * steps, as follows:
 * 
 *  1) Receive 'begin' signal from gameserver
 *      This signal is used to guarantee syncronization between the other players
 *      in order to prevent collisions. A collision would occur in the UWB modules
 *      due to only one message being able to be sent at one time across the entire 
 *      UWB network.The 'begin' signal will contain the tag-name of the player rifle
 *      and will be sent using MQTT
 * 
 *  2) Poll UWB location data
 *      Upon receiving the begin signal the player rifle will begin requesting and 
 *      receiving data from each of the 3 location anchors, sequentially. This 
 *      communication happens across the UWB network.
 * 
 *  3) Respond with 'finished' signal to gameserver
 *      This allows for the UWB network to not be delayed unnecessarily. 
 * 
 *  4) Poll IMU sensor for data
 *      This action is relatively short and can be treated as instantanous retrieval
 * 
 *  5) Push current raw-data to gameserver
 *      Using wifi, data should be pushed to the gameserver for processing.
 * 
 */

#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "main.h"

/////////////////////////////////////////
//          Compilation Flags          //
/////////////////////////////////////////
#define DEBUG
//#define MQTT_ACTIVE
//#define WIFI_ACTIVE
#define IMU_ACTIVE
#define UWB_ACTIVE
//#define RPI

/////////////////////////////////////////
//     Constants / Global Variables    //
/////////////////////////////////////////
struct IMU_samples_x3 ACC  = {0, 0, 0};
struct IMU_samples_x3 GYRO = {0, 0, 0};
struct IMU_samples_x3 MAG  = {0, 0, 0};
struct UWB_samples_x3 UWB  = {0, 0, 0};
#define TRIGGER_PIN     29      //actual pin 40
#define V_SOURCE_PIN    28      //actual pin 38
#define GND_PIN         28.5    //actual pin 39

#define PI              3.14159
#define DECLINATION     11.32   //magnetic declination for Logan UT in degrees
#define ACC_LSB         0.001F
#define ACC_FLAT	    1500
#define ACC_UP		    15500
#define ACC_DOWN 	    -16000

//hande compiling on not the PI
#ifdef RPI
    #include <wiringPi.h>
#else
    //RPI must be defined for actual operation. These prototypes allow for
    //development and compilation checks outside of the RPI.
    #define wiringPiSetup(VOID) 0
    #define digitalRead(pin) 0
    #define digitalWrite(pin, val) 0
    #define pinMode(pin, mode) 0
    #define INPUT 0
    #define OUTPUT 1
    #define LOW 0
    #define HIGH 1
#endif

/////////////////////////////////////////
//            IMU Functions            //
/////////////////////////////////////////
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
    //float zh = -MAG.x * cos(roll) * sin(pitch) + MAG.y * sin(roll) + MAG.z * cos(roll) * cos(pitch);

	float heading = 180 * atan2(yh, xh)/PI;
	if (yh < 0)
		heading += 36;
	printf("Pitch: %3.3f \tRoll: %3.3f \txh: %3.3f \tyh: %3.3f\n", pitch, roll, xh, yh);
	printf("Tilt: %3.3f\n", heading);
}

void print_MAG(){
	printf("Mag: %i %i %i\t", MAG.x, MAG.y, MAG.z);
	float heading = atan2(MAG.y, MAG.x);  // assume pitch, roll are 0
	heading *= 180 / PI;
	heading = heading + 180 - DECLINATION;
		
	printf("Heading: %3.4f ", heading);
	if(heading < 22.5)
		printf("EAST\n");
	else if(heading < 67.5)
		printf("NORTH-EAST\n");
	else if(heading < 112.5)
		printf("NORTH\n");
	else if(heading < 157.5)
		printf("NORTH-WEST\n");
	else if(heading < 202.5)
		printf("WEST\n");
	else if(heading < 247.5)
		printf("SOUTH-WEST\n");
	else if(heading < 292.5)
		printf("SOUTH\n");
	else if(heading < 337.5)
		printf("SOUTH-EAST\n");
	else
		printf("EAST\n");
}
/////////////////////////////////////////
//          wiringPi Functions         //
/////////////////////////////////////////
unsigned char pin_setup(){
    if (wiringPiSetup() < 0){
        printf("Failed to initialize wiring pi. Quitting\n");
        return 1;
    }
    pinMode(TRIGGER_PIN, INPUT);
    pinMode(V_SOURCE_PIN, OUTPUT);
    digitalWrite(V_SOURCE_PIN, HIGH);
    return 0;
}

unsigned char get_trigger(){
    return digitalRead(TRIGGER_PIN) & 0x1;
}

/////////////////////////////////////////
//           Comms Functions           //
/////////////////////////////////////////
void send_response(){
    //if we respond before we poll new values, other rifles wont have to 
    //wait while we are polling.
    #ifdef UWB_ACTIVE
        //poll the UWB modules for data
    #else
        printf("A: %i %i %i\t\tG: %i %i %i\t\tM: %i %i %i\n",
            ACC.x, ACC.y, ACC.z, GYRO.x, GYRO.y, GYRO.z, MAG.x, MAG.y, MAG.z);
        printf("A1: %3.3f\t\tA2: %3.3f\t\tA3: %3.3f", UWB.A1, UWB.A2, UWB.A3);
        printf("Shots Fired: %i\n\n", get_trigger());
    #endif

    #ifdef IMU_ACTIVE
        float* curr_samples = IMU_pull_samples();
        ACC.x = curr_samples[0];
        ACC.y = curr_samples[1];
        ACC.z = curr_samples[2];
        GYRO.x = curr_samples[3];
        GYRO.y = curr_samples[4];
        GYRO.z = curr_samples[5];
        MAG.x = curr_samples[6];
        MAG.y = curr_samples[7];
        MAG.z = curr_samples[8];
        
    #else
        //send default packet
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

    #ifdef MQTT_ACTIVE
        //do some MQTT
    #else
        printf("A: %i %i %i\t\tG: %i %i %i\t\tM: %i %i %i\n",
            ACC.x, ACC.y, ACC.z, GYRO.x, GYRO.y, GYRO.z, MAG.x, MAG.y, MAG.z);
        printf("A1: %3.3f\t\tA2: %3.3f\t\tA3: %3.3f", UWB.A1, UWB.A2, UWB.A3);
        printf("Shots Fired: %i\n\n", get_trigger());
    #endif
}

void alarmISR(int sig_num){
    if(sig_num == SIGALRM){
        //get UWB data

        //get IMU data

        #ifdef WIFI_ACTIVE
            //send responde over wifi
            send_response();
        #else
            //wifi is not enabled, print to screen

        #endif
    }
}

/////////////////////////////////////////
//                MAIN                 //
/////////////////////////////////////////
int main(){
    #ifdef RPI
        if(pin_setup()){
            printf("Failed to init wiring pi. Quitting\n");
            return 1;
        }
    #endif //RPI

    #ifdef IMU_ACTIVE
        #ifdef DEBUG
            printf("Begin IMU Initialization\n");
        #endif
        if(init_IMU())
            return 1;
        #ifdef DEBUG
            printf("Begin IMU Initialization\n");
        #endif
    #else
        #ifdef DEBUG
            printf("Skipping the IMU Initialization\n");
        #endif
    #endif  //IMU_ACTIVE

    //define synchronized trigger source
    #ifdef MQTT_ACTIVE
        //do something amazing here
    #else
        printf("MQTT not enabled. Timing will be defined by internal timer interrupts\n");
        //define the ISR called for the SIGALRM signal
        signal(SIGALRM, alarmISR);  // jumps to alarmISR as the ISR
        ualarm(500000, 500000);     // trigger a SIGALRM signal 1/2 second
    #endif  //MQTT_ACTIVE

    #ifdef WIFI_ACTIVE
        //send some message about how we're using wifi
    #else
        //wifi is not active, using stdout as output method
        #ifdef DEBUG
            printf("Wifi not enabled, sending output to stdout\n");
        #endif
    #endif
    
    //initialize the laser to be doing no work until told to do so
    while(1){
        #ifdef MQTT_ACTIVE
            //do something to wait for a 'begin' signal
        #endif
    }

    return 0;
}
