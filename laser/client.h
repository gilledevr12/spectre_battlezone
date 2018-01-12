#ifndef CLIENT_H
#define CLIENT_H
#include "lsm303.h"
#include "main.h"

int pull_DEVICE_MAC();
void config_client_socket();
void close_client_socket();
void send_status(struct int_x3 acc, struct int_x3 mag, int shot, int weight);


#endif //CLIENT_H
