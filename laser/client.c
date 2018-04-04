#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "client.h"

#define SERVER_PORT (int) 8080
#define SERVER_IP "192.168.0.5"

#define MAX_BUFFER_LENGTH 1024

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

void send_status(struct IMU_samples_x3 acc, /*struct IMU_samples_x3 gyro,*/ struct IMU_samples_x3 mag, struct UWB_samples_x3 uwb, uint8_t shots_fired){
    int packet_length;
    static char packet_buffer[200];
    sprintf(packet_buffer, "%s %i %i %i %i %i %i %3.2f %3.2f %3.2f %i", 
        DEVICE_MAC, acc.x, acc.y, acc.z, mag.x, mag.y, mag.z, uwb.A1, uwb.A2, uwb.A3, shots_fired);

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

char* receive_status(){
    static char buf[MAX_BUFFER_LENGTH];
    read(SOCK, buf, MAX_BUFFER_LENGTH);
    return buf;
}

void close_client_socket(){
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
