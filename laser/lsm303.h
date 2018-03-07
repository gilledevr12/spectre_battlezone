#ifndef LSM303_H
#define LSM303_H

#include <stdint.h>

void i2c_write_byte(uint8_t addr, uint8_t sub_addr, uint8_t val);
uint8_t i2c_read_byte(uint8_t addr, uint8_t sub_addr);

void init_acc();
uint16_t* read_acc();

void init_mag();
uint16_t* read_mag();

void init_imu();


#endif //LSM303_H
