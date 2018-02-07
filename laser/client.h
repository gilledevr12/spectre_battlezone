#ifndef CLIENT_H
#define CLIENT_H
//#include "lsm303.h"
#include "main.h"

int pull_DEVICE_MAC();
void open_client_socket();
void close_client_socket();
void send_status(struct float_x3 acc, struct float_x3 gyro, struct float_x3 mag, int shot, int weight);

#endif //CLIENT_H
