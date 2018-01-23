// Server side C/C++ program to demonstrate Socket programming
#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

pthread_t server_thread, gameplay_display_thread;

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
	char buffer[MAX_BUFFER_LENGTH] = {0};

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

	int count = 0;
	while(1){
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){
			perror("accept");
			exit(EXIT_FAILURE);
		}
		read(new_socket, buffer, MAX_BUFFER_LENGTH);
		printf("Received packet: %s\n",buffer );

		//	send(new_socket, server_resp, strlen(server_resp) ,0 );
	}
}

int main(){

	// This thread receives raw data, computes and generates valid data, sends out response
	pthread_create(&server_thread,NULL,server_loop,NULL);

	//build
	//pthread_create(&gameplay_display_thread,NULL,gameplay_display_loop,NULL);

	while(1);

  return 0;
}
