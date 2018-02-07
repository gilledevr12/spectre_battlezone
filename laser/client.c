#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "client.h"

#define SERVER_PORT (int) 8080
#define SERVER_IP "192.168.0.5"

char DEVICE_MAC[13];
int SOCK;

void open_client_socket(){
    struct sockaddr_in cli_addr;

    /*Build address data structure*/
    cli_addr.sin_family = AF_INET;
    inet_aton(SERVER_IP, &(cli_addr.sin_addr));
    cli_addr.sin_port = htons(SERVER_PORT);

    /*Active open*/
    if ((SOCK = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
      perror("client: error creating socket\n");
      close(SOCK);
      exit(1);
    }

    /*Connect to server*/
    if (connect(SOCK, (struct sockaddr*) &cli_addr, sizeof(cli_addr)) < 0)
    {
      perror("client: error establishing connection to server\n");
      close(SOCK);
      exit(1);
    }
}

void send_status(struct float_x3 acc, struct float_x3 gyro, struct float_x3 mag, int shot, int weight){
    /*Main loop: get/send lines of text*/
    int packet_length;
    char packet_buffer[120];
    sprintf(packet_buffer, "%s %3.5f %3.5f %3.5f %3.5f %3.5f %3.5f %3.5f %3.5f %3.5f %i %i", 
        DEVICE_MAC, acc.x, acc.y, acc.z, gyro.x, gyro.y, gyro.z, mag.x, mag.y, mag.z, shot, weight);

    #ifdef DEBUG
        printf("Sending client packet: %s\n", packet_buffer);
    #endif

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
        return 1;
    }

    fgets(DEVICE_MAC, 13, fin);
    #ifdef DEBUG
        printf("Pulled mac: %s\n", DEVICE_MAC);
    #endif
    fclose(fin);
    return 0;
}
