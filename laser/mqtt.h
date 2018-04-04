#ifndef MQTT_H
#define MQTT_H

/////////////////////////////////////////
//            MQTT Constants           //
/////////////////////////////////////////
#define MQTT_HOSTNAME "129.123.5.197" //change to the host name of the server
#define MQTT_PORT 1883
#define MQTT_TOPIC "location_sync"
#define MQTT_TOPIC_TAG "location_tag"
#define MQTT_TOPIC_INIT "location_init"

static char MQTT_NAME[10] = "Tag_";
static char MQTT_NAME_PUB[15] = "Pub_Tag_";
char round_match[6] = {0};
static struct mosquitto *mosq;
static struct mosquitto *mosq_pub;

int init_mosquitto();
int init_mosquitto_pub();

#endif /* MQTT_H */
