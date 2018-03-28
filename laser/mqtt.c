#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <mosquitto.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "mqtt.h"

/////////////////////////////////////////
//            MQTT Constants           //
/////////////////////////////////////////
#define MQTT_NAME "Server_Publisher"
#define MQTT_NAME_SUB "Server_Subscriber"
#define MQTT_PORT 1883
#define MQTT_TOPIC "location_sync"
#define MQTT_TOPIC_TAG "location_tag"