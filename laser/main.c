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

#include <stdbool.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "main.h"

/////////////////////////////////////////
//          Compilation Flags          //
/////////////////////////////////////////
#define DEBUG
#define MQTT_ACTIVE //UWB must also be enabled!
#define WIFI_ACTIVE
#define IMU_ACTIVE
#define UWB_ACTIVE  // MQTT must also be enabled!
#define RPI

/////////////////////////////////////////
//     Constants / Global Variables    //
/////////////////////////////////////////
struct IMU_samples_x3 ACC  = {1, 2, 3};
struct IMU_samples_x3 GYRO = {4, 5, 6};
struct IMU_samples_x3 MAG  = {7, 8, 9};
struct UWB_samples_x3 UWB  = {1.1, 2.2, 3.3}; //from anchors A1, A2 and A3 respectively
uint8_t SHOTS_FIRED;
volatile uint8_t POLL_SAMPLES = 0;
uint8_t MUTEX = 0;
uint8_t ready;

#define TRIGGER_PIN     29      //actual pin 40
#define V_SOURCE_PIN    28      //actual pin 38
#define GND_PIN         28.5    //actual pin 39

#define PI              3.14159
#define DECLINATION     11.32   //magnetic declination for Logan UT in degrees
#define ACC_LSB         0.001F
#define ACC_FLAT	    1500
#define ACC_UP		    15500
#define ACC_DOWN 	    -16000

//external variables

extern float UWB_Curr_Distance[3];

#ifdef MQTT_ACTIVE
    extern int anchCnt;
    extern bool quitting;
    extern bool success;
    extern bool nothingHappened;
    extern struct mosquitto *mosq;
    extern struct mosquitto *mosq_pub;
    extern char MQTT_NAME[10];
    extern char MQTT_NAME_PUB[15];
    extern uint8 rx_poll_msg[3][12];
    extern uint8 tx_resp_msg[3][15];
    extern uint8 rx_final_msg[3][24];
#else
    static bool quitting = 0;
#endif

//handle compiling on not the PI
#ifdef RPI
    #include <wiringPi.h>
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
    #ifdef RPI
        if (wiringPiSetup() < 0){
            printf("Failed to initialize wiring pi. Quitting\n");
            return 1;
        }
        pinMode(TRIGGER_PIN, INPUT);
        pinMode(V_SOURCE_PIN, OUTPUT);
        digitalWrite(V_SOURCE_PIN, HIGH);
        return 0;
    #else
        return 1;
    #endif
}

unsigned char get_trigger(){
    #ifdef RPI
        return digitalRead(TRIGGER_PIN) & 0x1;
    #else
        return 0;
    #endif
}

/////////////////////////////////////////
//           Comms Functions           //
/////////////////////////////////////////
void send_response(){
    	//printf("opening socket...");
	open_client_socket();
	//printf("done.\nsending packet...");
	send_status(ACC, /*GYRO,*/ MAG, UWB, SHOTS_FIRED);
	//printf("done.\nclosing socket...");
	close_client_socket();
	//printf("done\n");
}

void print_response(){
    printf("ACC: %i %i %i\n", ACC.x, ACC.y, ACC.z);
    printf("MAG: %i %i %i\n", MAG.x, MAG.y, MAG.z);
    printf("UWB: %3.2f %3.2f %4.2f\n", UWB.A1, UWB.A2, UWB.A3);
    if(SHOTS_FIRED)     printf("Shots fired!\n");
    else                printf("No shots fired\n");
    //reset SHOTS_FIRED reg
    SHOTS_FIRED = 0;
    printf("\n");
}

void alarmISR(int sig_num){
	if(sig_num == SIGALRM){
		POLL_SAMPLES = 1;
	//printf("restting alarm\n");
	alarm(1);
    }
}

/////////////////////////////////////////
//          DWM1000 Functions          //
/////////////////////////////////////////

void set_tag(int8_t id){
#ifdef MQTT_ACTIVE
    //printf("Which Tag am I? ");
    //char *bufNum = NULL;
    //size_t buf_size = 3;
    //getline(&bufNum, &buf_size, stdin);

    //strcat(MQTT_NAME, bufNum);
    //strcat(MQTT_NAME_PUB, bufNum);
    //for (int i = 0; i < 3; i++) {
    //    rx_poll_msg[i][8] = bufNum[0];
    //    tx_resp_msg[i][6] = bufNum[0];
    //    rx_final_msg[i][8] = bufNum[0];
    //}
    sprintf(MQTT_NAME, "%s%i", MQTT_NAME, id);
    sprintf(MQTT_NAME_PUB, "%s%i", MQTT_NAME_PUB, id);
    printf("%s\n%s\n", MQTT_NAME, MQTT_NAME_PUB);
    for(int i=0; i<3; i++){
	    rx_poll_msg[i][8] = MQTT_NAME[4];
        tx_resp_msg[i][6] = MQTT_NAME[4];
        rx_final_msg[i][8] = MQTT_NAME[4];
        //printf("%i\n%c\n%c\n%c\n\n", i, rx_poll_msg[i][8], tx_resp_msg[i][6], rx_final_msg[i][8]);
    }
    printf("\nI am %s\n", MQTT_NAME);
#endif
}

/////////////////////////////////////////
//                MAIN                 //
/////////////////////////////////////////
int main(int argc, char* argv[]){
    #ifdef RPI
        #ifdef DEBUG
        printf("Using RPI setup\n");
        #endif

        if(pin_setup()){
            printf("Failed to init wiring pi. Quitting\n");
            return 1;
        }
    #else
        #ifdef DEBUG
        printf("RPI not defined! Using external setup\n");
        #endif
    #endif //RPI

    #ifdef IMU_ACTIVE
        #ifdef DEBUG
        printf("Using IMU. Initializing...");
        #endif

        if(init_IMU()) return 1;

        #ifdef DEBUG
        printf("done!\n");
        #endif
    #else
        #ifdef DEBUG
        printf("IMU not defined. Skipping...\n");
        #endif
    #endif  //IMU_ACTIVE

    #ifdef UWB_ACTIVE
        #ifdef DEBUG
        printf("Using UWB. Initializing...");
        #endif

        if(init_dwm()) return 1;

        #ifdef DEBUG
        printf("done!\n");
        #endif
    #else
        #ifdef DEBUG
        printf("UWB not defined. Skipping...\n");
        #endif
    #endif  //UWB_ACTIVE

    //define synchronized trigger source
    #ifdef MQTT_ACTIVE
        #ifndef UWB_ACTIVE
            printf("MQTT can only be ran with UWB also active and functioning. Quitting..\n");
            return 1;
        #endif

        #ifdef DEBUG
        printf("MQTT enabled. Timing will be initiated by incoming MQTT prompts\n");
        #endif

	if(argc < 2){
		printf("Usage: ./laser-brains <tag-id>\n");
		printf("Quitting...\n");
		exit(1);
	}

	int8_t ID = atoi(argv[1]);
	if((ID != 1) || (ID != 2) || (ID != 3)) ID = 1;
        set_tag(ID);

	if(init_mosquitto()) return 1;
        if(init_mosquitto_pub()) return 1;

    #else
        #ifdef DEBUG
        printf("MQTT not enabled. Timing will be defined by internal timer interrupts\n");
        #endif
        //define the ISR called for the SIGALRM signal
        signal(SIGALRM, alarmISR);  // jumps to alarmISR as the ISR
        //ualarm(500000, 500000);     // trigger a SIGALRM signal 1/2 second
	alarm(1);	//set alarm to fire every 1 second
    #endif  //MQTT_ACTIVE

    #ifdef WIFI_ACTIVE
        #ifdef DEBUG
        printf("Wifi enabled! Output will be sent to gameserver\n");
        #endif
    #else
        #ifdef DEBUG
        //wifi is not active, using stdout as output method
        printf("Wifi not enabled, sending output to stdout\n");
        #endif
    #endif

    //initialize the laser to be doing no work until told to do so
    while(1){
        #ifdef MQTT_ACTIVE
        if (!success && !nothingHappened) {
            char buff[30];
            int tag = rx_final_msg[0][8] - '0';
            sprintf(buff, "Anchor%d Tag%d %s %s %s", anchCnt, tag, "play", "idle", "restart");
            if(mosquitto_publish(mosq_pub, NULL, MQTT_TOPIC, strlen(buff), buff, 0, false)){
                fprintf(stderr, "Could not publish to broker. Quitting\n");
                exit(-3);
            }
        } else if (nothingHappened){
            int tag = rx_final_msg[0][8] - '0';
            char token[8];
            if (anchCnt == 1){
                strcpy(token,"Anchor1");
            } else if (anchCnt == 2) {
                strcpy(token,"Anchor2");
            } else{
                strcpy(token,"Anchor3");
            }
            int num = token[strlen(token) - 1] - '0';
            while(!runRanging(token, num - 1, "play", "idle") && !quitting);
        }
        #endif
        while(!quitting) {
            #ifdef MQTT_ACTIVE
                //connect to MQTT broker
                int ret = mosquitto_loop(mosq, 250, 1);
                if (ret) {
                    fprintf(stderr, "Connection error. Reconnecting...\n");
                    sleep(1);
                    mosquitto_reconnect(mosq);
                }
                //when begin signal comes, data will be stored in global UWB_Curr_Distance
                //when all 3 samples have been taken, POLL_SAMPLES will be set to true

            #endif   //implied else -> POLL_SAMPLES flag set by AlarmISR

            if (POLL_SAMPLES) {
                POLL_SAMPLES = 0;
                #ifdef UWB_ACTIVE
                    UWB.A1 = UWB_Curr_Distance[0];
                    UWB.A2 = UWB_Curr_Distance[1];
                    UWB.A3 = UWB_Curr_Distance[2];
                #endif  //implied else - values initialized to 0

                //get IMU data
                #ifdef IMU_ACTIVE
                    int16_t *IMU_data = IMU_pull_samples_int();
                    ACC.x = IMU_data[0];
                    ACC.y = IMU_data[1];
                    ACC.z = IMU_data[2];
                    // GYRO.x  =   IMU_data[3];
                    // GYRO.y  =   IMU_data[4];
                    // GYRO.z  =   IMU_data[5];
                    MAG.x = IMU_data[3];
                    MAG.y = IMU_data[4];
                    MAG.z = IMU_data[5];
                #endif //implied else - values initialized to 0

                    //send out data
                #ifdef WIFI_ACTIVE
                    //send responde over wifi
                    send_response();
                #else
                static char packet_buffer[200];
                sprintf(packet_buffer, "%s %i %i %i %i %i %i %3.2f %3.2f %3.2f %i",
                    MQTT_NAME, ACC.x, ACC.y, ACC.z, MAG.x, MAG.y, MAG.z, UWB.A1, UWB.A2, UWB.A3, SHOTS_FIRED);
//                    if (mosquitto_publish(mosq_pub, NULL, MQTT_TOPIC_TAG, strlen(packet_buffer), packet_buffer, 0,
//                                          false)) {
//                        fprintf(stderr, "Could not publish to broker. Quitting\n");
//                        exit(-3);
//                    }
                    //wifi is not enabled, print to screen
                    print_response();
                #endif
            }

            //read trigger whenever possible. if pressed, store until next raw packet is sent
            #ifdef RPI
                if(!SHOTS_FIRED)
                    SHOTS_FIRED = get_trigger();
            #else
                SHOTS_FIRED = 0;
            #endif
        }
    #ifdef MQTT_ACTIVE
        //we'll get here if it took  too long to rx a message, if so, reset moquitto
        quitting = false;
        //shutdown and kill mosquitto
        mosquitto_destroy(mosq);
        mosquitto_destroy(mosq_pub);
        mosquitto_lib_cleanup();
        //restart
        init_dwm();
        init_mosquitto();
        init_mosquitto_pub();
    #endif
    }

    return 0;
}
