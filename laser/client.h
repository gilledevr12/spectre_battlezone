#ifndef CLIENT_H
#define CLIENT_H
//#include "lsm303.h"
#include "main.h"

//int pull_DEVICE_MAC();
void open_client_socket();
void close_client_socket();
void send_status(struct IMU_samples_x3 a, /*struct IMU_samples_x3 g,*/ struct IMU_samples_x3 m, struct UWB_samples_x3 u, uint8_t s);
char* receive_status();

#endif //CLIENT_H
 