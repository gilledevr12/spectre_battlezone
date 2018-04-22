#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "client.h"

#define SERVER_PORT (int) 3000
#define SERVER_IP "192.168.1.5"

#define MAX_BUFFER_LENGTH 1024

extern char MQTT_NAME[10];
char DEVICE_MAC[13];
int SOCK, TRUE = 1;
struct sockaddr sock_addr;
struct sockaddr_in *sock_addr_in = (struct sockaddr_in*) &sock_addr;

void open_client_socket(){
    /*Open socket: set type to active open*/
    SOCK = socket(PF_INET, SOCK_STREAM, 0);
    while(SOCK < 0){
        //if fail to bind to socket, reattempt until connection is made
        printf("SOCK errno: %s. Reattempting to bind socket\n", strerror(errno));
        sleep(1);
        SOCK = socket(PF_INET, SOCK_STREAM, 0);
        //perror("client: error creating socket\n");
        //close(SOCK);
        //exit(1);
    }

    int ret = setsockopt(SOCK ,SOL_SOCKET, SO_REUSEADDR, &TRUE, sizeof(int));
    if(ret < 0){
	    perror("setsockopt failed");
	    exit(1);
    }
    /*Build address data structure*/
    memset(&sock_addr, 0, sizeof(sock_addr));   //new change
    sock_addr_in->sin_family = PF_INET;
    sock_addr_in->sin_port = htons(SERVER_PORT);    //switched the ordering of this line with the one below
    inet_pton(PF_INET, SERVER_IP, &(sock_addr_in->sin_addr));

    /*Connect to server*/
    ret = connect(SOCK, (struct sockaddr*) &sock_addr, sizeof(sock_addr));
    while(ret < 0){
        printf("Connect errno: %s. Reattempting connection.\n", strerror(errno));
        sleep(1);
        ret = connect(SOCK, (struct sockaddr*) &sock_addr, sizeof(sock_addr));
        //close(SOCK);
        //exit(1);
    }
}

void send_status(struct IMU_samples_x3 acc, /*struct IMU_samples_x3 gyro,*/ struct IMU_samples_x3 mag, struct UWB_samples_x3 uwb, uint8_t shots_fired){
    int packet_length;
    static char packet_buffer[200];
    sprintf(packet_buffer, "%s %i %i %i %i %i %i %3.2f %3.2f %3.2f %i",
            MQTT_NAME, acc.x, acc.y, acc.z, mag.x, mag.y, mag.z, uwb.A1, uwb.A2, uwb.A3, shots_fired);

    #ifdef DEBUG
        printf("Sending client packet: %s\n", packet_buffer);
    #endif

    packet_length = strlen(packet_buffer) + 1;

    if(sendto(SOCK, packet_buffer, packet_length, 0, &sock_addr, sizeof(sock_addr_in)) <= 0)
    {
        /*close(s);*/
        perror("client: error sending packet \n");
        exit(1);
    }
}

char* receive_status(){
    static char buf[MAX_BUFFER_LENGTH];
    read(SOCK, buf, MAX_BUFFER_LENGTH);
    return buf;
}

void close_client_socket(){
    shutdown(SOCK, SHUT_RDWR);
    close(SOCK);
}

// int pull_DEVICE_MAC(){
//     FILE* fin;
//     fin = fopen("DEVICE_MAC", "r");
//     if(fin < 0){
//         perror("Run gen_mac_file.sh before launching this program. Quitting..\n");
//         return 1;
//     }

//     fgets(DEVICE_MAC, 13, fin);
//     #ifdef DEBUG
//         printf("Pulled mac: %s\n", DEVICE_MAC);
//     #endif
//     fclose(fin);
//     return 0;
// }
