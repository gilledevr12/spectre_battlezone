#include <errno.h>
#include <fcntl.h>
#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Server connection parameters
#define MQTT_HOSTNAME "129.123.5.197"
#define MQTT_NAME "Server_Publisher"
#define MQTT_PORT 1883
#define MQTT_TOPIC "location_sync"

int main(){
    
    mosquitto_lib_init();
    struct mosquitto *mosq = mosquitto_new(MQTT_NAME, true, NULL);
    if(!mosq){
        fprintf(stderr, "Could not initialize mosquitto library. Quitting\n");
        exit(-1);
    }

    if(mosquitto_connect(mosq, MQTT_HOSTNAME, MQTT_PORT, 0)){
        fprintf(stderr, "Could not connect to mosquitto broker. Quitting\n");
        exit(-2);
    }

    char buf[7];

    while(1){
        sprintf(buf, "Round%i", 1);
        if(mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(buf), buf, 0, false)){
            fprintf(stderr, "Could not publish to broker. Quitting\n");
            exit(-3);
        }
        sleep(1);
        
        sprintf(buf, "Round%i", 2);
        if(mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(buf), buf, 0, false)){
            fprintf(stderr, "Could not publish to broker. Quitting\n");
            exit(-3);
        }
        sleep(1);

        sprintf(buf, "Round%i", 3);
        if(mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(buf), buf, 0, false)){
            fprintf(stderr, "Could not publish to broker. Quitting\n");
            exit(-3);
        }
        sleep(1);
    }

    mosquitto_disconnect (mosq);
    mosquitto_destroy (mosq);
    mosquitto_lib_cleanup();

    return 0;
}