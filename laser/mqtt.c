#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <mosquitto.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "mqtt.h"
#include "dwm1000.h"

char round_match[6] = {0};

void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
    bool match = 0;
    bool matchTag = 0;

    const char s[2] = " ";
    char *token;
    char *tag;
    char *play;
    char *poll;

    //printf("got message '%.*s' for topic '%s'\n", message->payloadlen, (char*) message->payload, message->topic);
    /* get the first token */
    token = strtok((char*) message->payload, s);
    if ( token != NULL ) {
        tag = strtok(NULL, s);
    }
    if ( tag != NULL ) {
        play = strtok(NULL, s);
    }
    if ( play != NULL ) {
        poll = strtok(NULL, s);
    }

    sprintf(round_match, "Tag%c", rx_poll_msg[0][8]);

    mosquitto_topic_matches_sub(MQTT_TOPIC, message->topic, &match);
    if (match) {
        mosquitto_topic_matches_sub(round_match, tag, &matchTag);
        if (matchTag){
            int num = token[strlen(token) - 1] - '0';
            runRanging(token, num - 1, play, poll);
        }
        //printf("got message for %s topic\n", MQTT_TOPIC);
    }
}

int init_mosquitto(){
    mosquitto_lib_init();
    mosq = mosquitto_new(MQTT_NAME, true, NULL);
    if (!mosq) {
        printf("Could not initialize mosquitto library. Quitting\n");
        return 1;
    }

    if (mosquitto_connect(mosq, MQTT_HOSTNAME, MQTT_PORT, 0)) {
        printf("Could not connect to mosquitto broker. Quitting\n");
        return 1;
    }

    char buf[7];

    mosquitto_message_callback_set(mosq, message_callback);
    mosquitto_subscribe(mosq, NULL, MQTT_TOPIC, 0);
    return 0;
}

int init_mosquitto_pub(){
    mosq_pub = mosquitto_new(MQTT_NAME_PUB, true, NULL);
    if (!mosq_pub) {
        printf("Could not initialize mosquitto library. Quitting\n");
        return 1;
    }

    if (mosquitto_connect(mosq_pub, MQTT_HOSTNAME, MQTT_PORT, 0)) {
        printf("Could not connect to mosquitto broker. Quitting\n");
        return 1;
    }
}