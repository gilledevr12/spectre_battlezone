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
#define MQTT_NAME "Tag_"
#define MQTT_NAME_PUB "Pub_Tag_"

static struct mosquitto *mosq;
static struct mosquitto *mosq_pub;

int init_mosquitto();
int init_mosquitto_pub();

#endif /* MQTT_H */
