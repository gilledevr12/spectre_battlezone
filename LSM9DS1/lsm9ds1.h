#ifndef LSM9DS1_H
#define LSM9DS1_H

/* GYROSCOPE Functions */
void init_gyro();
int* read_gyro();

/* ACCELEROMETER Functions */
void init_accel();
int* read_accel();

/* MAGNETOMETER Functions */
void init_mag();
int* read_mag();

/* General IMU Functions */
unsigned char imu_read_byte(unsigned char addr, unsigned char sub_addr);
void imu_write_byte(unsigned char addr, unsigned char sub_addr, unsigned char val);
void calibrate_IMU();
char init_IMU();
int* IMU_pull_samples();

#endif  //LSM9DS1_H