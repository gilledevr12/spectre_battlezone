// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#include "client.h"
#include "DEVICE_MAC.h"
#include "lsm303.h"

#define PORT 8080
#define SERVER_IP "192.168.0.5"

struct int_x3 ACCEL, MAGNETOM;

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

    char packet_buffer[100];

    int I2C_PORT = open_I2C_port("/dev/i2c-1");
    config_LSM303(I2C_PORT);
    ACCEL = get_accel(I2C_PORT);
    MAGNETOM = get_magnetom(I2C_PORT);

        sprintf(packet_buffer, "MAC: %lu", DEVICE_MAC);
        send(sock, packet_buffer, strlen(client_packet), 0);
	usleep(50);

        sprintf(packet_buffer, "ACC: %i %i %i", ACCEL.x, ACCEL.y, ACCEL.z);
        send(sock, packet_buffer, strlen(client_packet), 0);
	usleep(50);

        sprintf(packet_buffer, "MAG: %i %i %i", MAGNETOM.x, MAGNETOM.y, MAGNETOM.z);
        send(sock, packet_buffer, strlen(client_packet), 0);

    // printf("Client packet sent\n");
    valread = read( sock, buffer, 1024);
    printf("%s\n", buffer);
    return 0;
}
