#ifndef LSM9DS1_H
#define LSM9DS1_H

/* GYROSCOPE Functions */
void init_gyro();

/* ACCELEROMETER Functions */
void init_accel();

/* MAGNETOMETER Functions */
void init_mag();
void calibrate_mag();

/* General IMU Functions */
void read_device_bytes(unsigned char addr, unsigned char sub_addr, short* dest);
unsigned char imu_read_byte(unsigned char addr, unsigned char sub_addr);
void imu_write_byte(unsigned char addr, unsigned char sub_addr, unsigned char val);
void calibrate_IMU();
char init_IMU();
float* IMU_pull_samples();
void read_memory();

#endif  //LSM9DS1_H