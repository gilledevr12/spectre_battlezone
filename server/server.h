#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>

void gameplay_display_loop();
void server_loop();

struct raw_data_packet{
    char* ID;
    int16_t ACCX;
    int16_t ACCY;
    int16_t ACCZ;
    int16_t MAGX;
    int16_t MAGY;
    int16_t MAGZ;
    float UWBA1;
    float UWBA2;
    float UWBA3;
    uint8_t FIRED;
};

#endif
