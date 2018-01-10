// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#include "client.h"
#include "DEVICE_MAC.h"

#define PORT 8080
#define SERVER_IP "192.168.0.5"

int main(int argc, char const *argv[]){
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *client_packet = "Client packet data";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr)<=0){
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("\nConnection Failed \n");
        return -1;
    }

    char packet_buffer[50];

    int traj_x = 100, traj_y = 150, traj_z = 50;

    for(int i=0; i<10; i++)
    {
        sprintf(packet_buffer, "%i %i %i %i\n", DEVICE_MAC, traj_x, traj_y, traj_z);
	printf("Sending packet: %i %i %i %i\n", DEVICE_MAC, traj_x, traj_y, traj_z);
	send(sock, packet_buffer, strlen(client_packet), 0);
        usleep(500);
    }

    // send(sock, client_packet, strlen(client_packet), 0);

    // printf("Client packet sent\n");
    valread = read( sock, buffer, 1024);
    printf("%s\n", buffer);
    return 0;
}
