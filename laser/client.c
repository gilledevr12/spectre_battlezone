#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "client.h"

#define SERVER_PORT (int) 8080
//#define SERVER_IP "192.168.0.5"
#define SERVER_IP "144.39.166.226"

char DEVICE_MAC[13];
int SOCK;

void config_client_socket(){
    struct sockaddr_in sock_in;

    /*Build address data structure*/
    sock_in.sin_family = AF_INET;
    inet_aton(SERVER_IP, &(sock_in.sin_addr));
    sock_in.sin_port = htons(SERVER_PORT);

    /*Active open*/
    if ((SOCK = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
      perror("client: error creating socket\n");
      close(SOCK);
      exit(1);
    }

    /*Connect to server*/
    if (connect(SOCK, (struct sockaddr*) &sock_in, sizeof(sock_in)) < 0)
    {
      perror("client: error establishing connection to server\n");
      close(SOCK);
      exit(1);
    }
}

void send_status(struct int_x3 acc, struct int_x3 mag, int shot, int weight){
    /*Main loop: get/send lines of text*/
    int packet_length;
    char packet_buffer[100];
    sprintf(packet_buffer, "%s %i %i %i %i %i %i, %i, %i", DEVICE_MAC, acc.x, acc.y, acc.z, mag.x, mag.y, mag.z, shot, weight);

    if(DEBUG)
        printf("Sending client packet: %s\n", packet_buffer);

    packet_length = strlen(packet_buffer) + 1;

    if(send(SOCK, packet_buffer, packet_length, 0) <= 0)
    {
        /*close(s);*/
        perror("client: error sending packet \n");
        exit(1);
    }
}

void close_client_socket(){
    close(SOCK);
}

int pull_DEVICE_MAC(){
    FILE* fin;
    fin = fopen("DEVICE_MAC", "r");
    if(fin < 0){
        perror("Run gen_mac_file.sh before launching this program. Quitting..\n");
        return 0;
    }

    fgets(DEVICE_MAC, 13, fin);
    if(DEBUG)
        printf("Pulled mac: %s\n", DEVICE_MAC);
    fclose(fin);
    return 1;
}
