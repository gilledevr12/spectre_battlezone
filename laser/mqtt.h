#ifndef MQTT_H
#define MQTT_H

/////////////////////////////////////////
//            MQTT Constants           //
/////////////////////////////////////////
#define MQTT_HOSTNAME "192.168.0.5" //change to the host name of the server
#define MQTT_PORT 1883
#define MQTT_TOPIC "location_sync"
#define MQTT_TOPIC_TAG "location_tag"
#define MQTT_TOPIC_INIT "location_init"

struct mosquitto *mosq;
struct mosquitto *mosq_pub;

int init_mosquitto();
int init_mosquitto_pub();

#endif /* MQTT_H */
