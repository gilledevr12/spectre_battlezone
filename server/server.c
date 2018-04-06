// Server side C/C++ program to demonstrate Socket programming
#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include "server.h"
#include "setup.h"
extern struct location_data loc_dat;

pthread_t server_thread, gameplay_display_thread;
char* PLAYER_ID;
uint16_t ACC[3];

#define PORT 8080
#define MAX_BUFFER_LENGTH 1024

void gameplay_display_loop(){
	//do nothing for now
	while(1);
}

void server_loop(){
	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char* buffer;
	PLAYER_ID = (char*) malloc(15 * sizeof(char));
	buffer = (char*) malloc( MAX_BUFFER_LENGTH * sizeof(char));

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0){
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 5) < 0){
		perror("listen");
		exit(EXIT_FAILURE);
	}

	printf("Server launch\n");
	char* token;
	struct raw_data_packet tmp_pkt;
	while(1){
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){
			perror("accept");
			exit(EXIT_FAILURE);
		}
		read(new_socket, buffer, MAX_BUFFER_LENGTH);
		printf("Rx:  %s\n", buffer);
		token = strtok(buffer, " ");		
		tmp_pkt.ID = token;		
		token = strtok(NULL, " ");
		tmp_pkt.ACCX = atoi(token);		
		token = strtok(NULL, " ");
		tmp_pkt.ACCY = atoi(token);
		token = strtok(NULL, " ");
		tmp_pkt.ACCZ = atoi(token);		
		token = strtok(NULL, " ");
		tmp_pkt.MAGX = atoi(token);		
		token = strtok(NULL, " ");
		tmp_pkt.MAGY = atoi(token);		
		token = strtok(NULL, " ");
		tmp_pkt.MAGZ = atoi(token);		
		token = strtok(NULL, " ");
		tmp_pkt.UWBA1 = atof(token);		
		token = strtok(NULL, " ");
		tmp_pkt.UWBA2 = atof(token);
		token = strtok(NULL, " ");
		tmp_pkt.UWBA3 = atof(token);
		token = strtok(NULL, " ");
		tmp_pkt.FIRED = atoi(token);
		printf("Act: %s %i %i %i %i %i %i %3.3f %3.3f %3.3f ",
			tmp_pkt.ID,
			tmp_pkt.ACCX,  tmp_pkt.ACCY,  tmp_pkt.ACCZ,
			tmp_pkt.MAGX,  tmp_pkt.MAGY,  tmp_pkt.MAGZ,
			tmp_pkt.UWBA1, tmp_pkt.UWBA2, tmp_pkt.UWBA3);
		if(tmp_pkt.FIRED)
			printf("FIRED!\n");
		else
			printf("no shots fired\n");
	}
}

// int main(){

// }
