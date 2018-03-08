#ifndef LSM303_H
#define LSM303_H

#include <stdint.h>

void i2c_write_byte(uint8_t addr, uint8_t sub_addr, uint8_t val);
uint8_t i2c_read_byte(uint8_t addr, uint8_t sub_addr);

void init_acc();
int16_t* read_acc();

void init_mag();
int16_t* read_mag();

void calibrate_IMU();
void init_imu();
int16_t* LSM303_IMU_pull_samples();
void print_memory();

#endif //LSM303_H
