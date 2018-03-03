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
#define MQTT_NAME_SUB "Server_Subscriber"
#define MQTT_PORT 1883
#define MQTT_TOPIC "location_sync"
#define MQTT_TOPIC_TAG "location_tag"

static struct mosquitto *mosq; 

void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
	bool match = 0;
	printf("got message '%.*s' for topic '%s'\n", message->payloadlen, (char*) message->payload, message->topic);

	mosquitto_topic_matches_sub(MQTT_TOPIC_TAG, message->topic, &match);
	if (match) {
		printf("got message for %s topic\n", MQTT_TOPIC);
	}
}

int main(){
    
    mosquitto_lib_init();
    mosq = mosquitto_new(MQTT_NAME, true, NULL);
    if(!mosq){
        fprintf(stderr, "Could not initialize mosquitto library. Quitting\n");
        exit(-1);
    }

    struct mosquitto *mosq_sub = mosquitto_new(MQTT_NAME_SUB, true, NULL);
    if(!mosq_sub){
        fprintf(stderr, "Could not initialize mosquitto library. Quitting\n");
        exit(-1);
    }

    if(mosquitto_connect(mosq, MQTT_HOSTNAME, MQTT_PORT, 0)){
        fprintf(stderr, "Could not connect to mosquitto broker. Quitting\n");
        exit(-2);
    }

    if(mosquitto_connect(mosq_sub, MQTT_HOSTNAME, MQTT_PORT, 0)){
        fprintf(stderr, "Could not connect to mosquitto broker. Quitting\n");
        exit(-2);
    }

    mosquitto_message_callback_set(mosq_sub, message_callback);
    mosquitto_subscribe(mosq_sub, NULL, MQTT_TOPIC_TAG, 1);

    char buf[7];

    while(1){
        int ret = mosquitto_loop(mosq_sub, 250, 1); //different thread?
        if(ret){
		    fprintf(stderr, "Connection error. Reconnecting...\n");
		    sleep(1);
		    mosquitto_reconnect(mosq_sub);
	    }
        sprintf(buf, "Round%i", 1);
        if(mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(buf), buf, 0, false)){
            fprintf(stderr, "Could not publish to broker. Quitting\n");
            exit(-3);
        }
        ret = mosquitto_loop(mosq_sub, 250, 1); //different thread?
        if(ret){
		    fprintf(stderr, "Connection error. Reconnecting...\n");
		    sleep(1);
		    mosquitto_reconnect(mosq_sub);
	    }
        usleep(200000);
        
        sprintf(buf, "Round%i", 2);
        if(mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(buf), buf, 0, false)){
            fprintf(stderr, "Could not publish to broker. Quitting\n");
            exit(-3);
        }
        ret = mosquitto_loop(mosq_sub, 250, 1); //different thread?
        if(ret){
		    fprintf(stderr, "Connection error. Reconnecting...\n");
		    sleep(1);
		    mosquitto_reconnect(mosq_sub);
	    }
        usleep(200000);

        //add when 6 tags + anchors are used

        // sprintf(buf, "Round%i", 3);
        // if(mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(buf), buf, 0, false)){
        //     fprintf(stderr, "Could not publish to broker. Quitting\n");
        //     exit(-3);
        // }
        // ret = mosquitto_loop(mosq_sub, 250, 1); //different thread?
        // if(ret){
		//     fprintf(stderr, "Connection error. Reconnecting...\n");
		//     sleep(1);
		//     mosquitto_reconnect(mosq_sub);
	    // }
        // usleep(500000);
    }

    mosquitto_disconnect (mosq);
    mosquitto_destroy (mosq);
    mosquitto_lib_cleanup();

    return 0;
}