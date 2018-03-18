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
#define MQTT_TOPIC_INIT "location_init"

static struct mosquitto *mosq;
static double a1_x;
static double a1_y;
static double a2_x;
static double a2_y;
static double a3_x;
static double a3_y;
static double a1_const;
static double a2_const;
static double a3_const;
static double dist_buf[10];
static int index;
static bool access;

void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
    bool match = 0;
    //printf("got message '%.*s' for topic '%s'\n", message->payloadlen, (char*) message->payload, message->topic);
    mosquitto_topic_matches_sub(MQTT_TOPIC_TAG, message->topic, &match);
    if (match) {
        double d1;
        double d2;
        double d3;
        int tag = 0;
        const char s[2] = " ";
        char *token;
        /* get the first token */
        token = strtok((char*) message->payload, s);
        int cnt = 0;
        bool setTag = false;
        /* walk through other tokens */
        while( token != NULL ) {
            //printf( "%s\n", token);
            if (strcmp(token,"m") != 0) cnt += 1;
            if (setTag){
                tag = atoi(token);
                setTag = false;
            }
            if (strcmp(token,"Tag:") == 0) setTag = true;
            if (!(cnt % 6)) {
                if ((cnt / 6) == 1){
                    d1 = pow(atof(token),2.0);
                } else if ((cnt / 6) == 2){
                    d2 = pow(atof(token),2.0);
                } else if ((cnt / 6) == 3) {
                    d3 = pow(atof(token),2.0);
                }
            }
            token = strtok(NULL, s);
        }

        double eq1_x = a1_x - a2_x;
        double eq1_y = a1_y - a2_y;
        double eq2_x = a1_x - a3_x;
        double eq2_y = a1_y - a3_y;
        double eq1_const = a1_const - a2_const;
        double eq2_const = a1_const - a3_const;
        double eq1_f = d1 - d2 - eq1_const;
        double eq2_f = d1 - d3 - eq2_const;
        double x = (eq2_f - ((eq2_y*eq1_f)/eq1_y))/(eq2_x - ((eq2_y*eq1_x)/eq1_y));
        double y = (eq1_f - (eq1_x * x))/(eq1_y);
        printf("Tag %d position: (%f, %f)\n", tag, x, y);
        //printf("%.*s\n", message->payloadlen, (char*) message->payload);
        //printf("got message for %s topic\n", MQTT_TOPIC);
    }
}

void message_callback_init(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
    bool match = 0;
    index += 1;
    //printf("got message '%.*s' for topic '%s'\n", message->payloadlen, (char*) message->payload, message->topic);
    mosquitto_topic_matches_sub(MQTT_TOPIC_INIT, message->topic, &match);
    if (match) {
        double d1;
        int tag = 0;
        const char s[2] = " ";
        char *token;
        /* get the first token */
        token = strtok((char*) message->payload, s);
        int cnt = 0;
        bool setTag = false;
        /* walk through other tokens */
        while( token != NULL ) {
            //printf( "%s\n", token);
            if (strcmp(token,"m") != 0) cnt += 1;
            if (setTag){
                tag = atoi(token);
                setTag = false;
            }
            if (strcmp(token,"Tag:") == 0) setTag = true;
            if (!(cnt % 6)) {
                if ((cnt / 6) == 1){
                    d1 = pow(atof(token),2.0);
                }
            }
            token = strtok(NULL, s);
        }

        dist_buf[index] = d1;

        if (index == 9) access = true;

        //printf("%.*s\n", message->payloadlen, (char*) message->payload);
        //printf("got message for %s topic\n", MQTT_TOPIC);
    }
}

void getDist(struct mosquitto *mosq, struct mosquitto *mosq_sub, int anchor, char axis){
    size_t BLEN = 16;
    char* buf = 0;
    int tag = 1;
    //prep in your spot
    printf("Position the tag please for Anchor %d %c position \n", anchor, axis);
    usleep(10000000);
    int ret = mosquitto_loop(mosq_sub, 250, 1); //different thread?
    index = -1;
    for(int i = 0; i < 10; i++) {
        if(ret){
            fprintf(stderr, "Connection error. Reconnecting...\n");
            sleep(1);
            mosquitto_reconnect(mosq_sub);
        }
        sprintf(buf, "Anchor%d Tag%d", anchor, tag);
        if(mosquitto_publish(mosq, NULL, MQTT_TOPIC_INIT, strlen(buf), buf, 0, false)){
            fprintf(stderr, "Could not publish to broker. Quitting\n");
            exit(-3);
        }
        ret = mosquitto_loop(mosq_sub, 250, 1); //different thread?
        usleep(40000);
    }
//    usleep(1000000);
    //wait to access buffer
    while (!access) {};
    double total;
    for (int i = 0; i < 10; i++){
        total += dist_buf[i];
    }
    total = total/10;
    access = false;
    printf("Anchor %d %c position in meters (2 decimal points): ", anchor, axis);
//    getline(&buf, &BLEN, stdin);
    return total;
}

int main(){

    index = 0;
    access = false;
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
    mosquitto_message_callback_set(mosq_sub, message_callback_init);
    mosquitto_subscribe(mosq_sub, NULL, MQTT_TOPIC_TAG, 1);

    double a1_x_dist = getDist(mosq, mosq_sub, 1, 'x');
    a1_x = a1_x_dist * (-1) * 2;
    double a1_y_dist = getDist(mosq, mosq_sub, 1, 'y');
    a1_y = a1_y_dist * (-1) * 2;
    a1_const = pow(a1_x_dist,2) + pow(a1_y_dist,2);

    double a2_x_dist = getDist(mosq, mosq_sub, 2, 'x');
    a2_x = a2_x_dist * (-1) * 2;
    double a2_y_dist = getDist(mosq, mosq_sub, 2, 'y');
    a2_y = a2_y_dist * (-1) * 2;
    a2_const = pow(a2_x_dist,2) + pow(a2_y_dist,2);

    double a3_x_dist = getDist(mosq, mosq_sub, 3, 'x');
    a3_x = a3_x_dist * (-1) * 2;
    double a3_y_dist = getDist(mosq, mosq_sub, 3, 'y');
    a3_y = a3_y_dist * (-1) * 2;
    a3_const = pow(a3_x_dist,2) + pow(a3_y_dist,2);

    size_t BLEN = 16;
    char* buf = 0;
    int tagCnt = 1;
    int anchorCnt = 1;

//    printf("Anchor 1 x position in meters (2 decimal points): ");
//    getline(&buf, &BLEN, stdin);
//    double a1_x_dist = atof(buf);
//    a1_x = a1_x_dist * (-1) * 2;
//    printf("Anchor 1 y position in meters (2 decimal points): ");
//    getline(&buf, &BLEN, stdin);
//    double a1_y_dist = atof(buf);
//    a1_y = a1_y_dist * (-1) * 2;
//    a1_const = pow(a1_x_dist,2) + pow(a1_y_dist,2);
//    printf("Anchor 2 x position in meters (2 decimal points): ");
//    getline(&buf, &BLEN, stdin);
//    double a2_x_dist = atof(buf);
//    a2_x = a2_x_dist * (-1) * 2;
//    printf("Anchor 2 y position in meters (2 decimal points): ");
//    getline(&buf, &BLEN, stdin);
//    double a2_y_dist = atof(buf);
//    a2_y = a2_y_dist * (-1) * 2;
//    a2_const = pow(a2_x_dist,2) + pow(a2_y_dist,2);
//    printf("Anchor 3 x position in meters (2 decimal points): ");
//    getline(&buf, &BLEN, stdin);
//    double a3_x_dist = atof(buf);
//    a3_x = a3_x_dist * (-1) * 2;
//    printf("Anchor 3 y position in meters (2 decimal points): ");
//    getline(&buf, &BLEN, stdin);
//    double a3_y_dist = atof(buf);
//    a3_y = a3_y_dist * (-1) * 2;
//    a3_const = pow(a3_x_dist,2) + pow(a3_y_dist,2);

    while(1){
        int ret = mosquitto_loop(mosq_sub, 250, 1); //different thread?
        if(ret){
            fprintf(stderr, "Connection error. Reconnecting...\n");
            sleep(1);
            mosquitto_reconnect(mosq_sub);
        }
        sprintf(buf, "Anchor%d Tag%d", anchorCnt, tagCnt);
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
        usleep(40000);

        anchorCnt++;
        sprintf(buf, "Anchor%d Tag%d", anchorCnt, tagCnt);
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
        usleep(40000);

        //add when 6 tags + anchors are used

        anchorCnt++;
        sprintf(buf, "Anchor%d Tag%d", anchorCnt, tagCnt);
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
        usleep(40000);

        anchorCnt = 1;
        tagCnt++;
        if (tagCnt == 4){
            tagCnt = 1;
        }
    }

    mosquitto_disconnect (mosq);
    mosquitto_destroy (mosq);
    mosquitto_lib_cleanup();

    return 0;
}

