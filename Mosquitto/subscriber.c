#include <errno.h>
#include <fcntl.h>
#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Server connection parameters
#define MQTT_HOSTNAME "129.123.5.197"
#define MQTT_NAME "Server_Subscriber"
#define MQTT_PORT 1883
#define MQTT_TOPIC "location_sync"

void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
	bool match = 0;
	printf("got message '%.*s' for topic '%s'\n", message->payloadlen, (char*) message->payload, message->topic);

	mosquitto_topic_matches_sub(MQTT_TOPIC, message->topic, &match);
	if (match) {
		printf("got message for %s topic\n", MQTT_TOPIC);
	}
}

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

    mosquitto_message_callback_set(mosq, message_callback);
    mosquitto_subscribe(mosq, NULL, MQTT_TOPIC, 1);

    while(1){
	int ret = mosquitto_loop(mosq, -1, 1);
	if(ret){
		fprintf(stderr, "Connection error. Reconnecting...\n");
		sleep(1);
		mosquitto_reconnect(mosq);
	}
    }
	    
    mosquitto_destroy (mosq);
    mosquitto_lib_cleanup();

    return 0;
}
