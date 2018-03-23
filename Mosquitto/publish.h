#ifndef PUBLISH_H
#define PUBLISH_H
#include <errno.h>
#include <fcntl.h>
#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

// Server connection parameters
#define MQTT_HOSTNAME "129.123.5.197"
#define MQTT_NAME "Server_Publisher"
#define MQTT_NAME_SUB "Server_Subscriber"
#define MQTT_PORT 1883
#define MQTT_TOPIC "location_sync"
#define MQTT_TOPIC_TAG "location_tag"

void message_callback(struct mosquitto*, void*, const struct mosquitto_message*);

void message_callback_init(struct mosquitto*, void*, const struct mosquitto_message*);

double getDist(struct mosquitto*, struct mosquitto*, int, char);

#endif
