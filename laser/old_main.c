// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
// #include <wiringPi.h>

#include "main.h"

#define TRIGGER_PIN 26
#define V_SOURCE_PIN 25

volatile unsigned char trig_press;

char wiringPiSetup();
char digitalRead(char p);
void digitalWrite(char p, char v);
void pinMode(char c, char m);
#define INPUT 0
#define OUTPUT 1

char CLIENT_STATUS;
//  status descriptions
//      0: GAMEPLAY NORMAL OPERATING MODE
//      1: HALT - client is in halt mode, waiting for valid response
//      2: NA

struct float_x3 ACCEL, GYRO, MAG;

void alarmISR(int sig_num){
    #ifdef DEBUG
        printf("ISR called for signal: %i\n", sig_num);
    #endif
    if(sig_num == SIGALRM){
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

        #ifdef CLIENT_ENABLE
            open_client_socket();
            send_status(ACCEL, GYRO, MAG, trig_press, 0); 
            close_client_socket();
        #else
            printf("A:%2.5f %2.5f %2.5f \t\tG:%2.5f %2.5f %2.5f \t\tM:%2.5f %2.5f %2.5f\n",
                ACCEL.x, ACCEL.y, ACCEL.z, GYRO.x, GYRO.y, GYRO.z, MAG.x, MAG.y, MAG.z);
        #endif

        trig_press = 0;
    }
}

int main(){

    if(wiringPiSetup() < 0){
        printf("Wiring pi setup failed. Quitting\n");
        return 1;
    }
    pinMode(TRIGGER_PIN, INPUT);
    pinMode(V_SOURCE_PIN, OUTPUT);
    digitalWrite(V_SOURCE_PIN, HIGH);

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

    //define the ISR called for the SIGALRM signal
    signal(SIGALRM, alarmISR);  // jumps to alarmISR as the ISR
    ualarm(250000, 250000);     // trigger a SIGALRM signal every 1/4 second
    
    //initialize the laser to be doing no work until told to do so
    CLIENT_STATUS = 1;          

    trig_press = 0;
    while(1){
        if(!trig_press) //only continue to sample if it hasn't already been pressed
            if(digitalRead(TRIGGER_PIN))
                trig_press = 0b1;
    }

    return 0;
}
