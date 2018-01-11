#ifndef LSM303_H
#define LSM303_H

struct int_x3 {
    int x;
    int y;
    int z;
};

int open_I2C_port(char* path);

void config_LSM303(int bus);
struct int_x3 get_accel(int bus);
struct int_x3 get_magnetom(int bus);

void config_accelerometer(int bus);
int get_accel_x(int bus);
int get_accel_y(int bus);
int get_accel_z(int bus);

void config_magnetometer(int bus);
int get_magnetom_x(int bus);
int get_magnetom_y(int bus);
int get_magnetom_z(int bus);

#endif //LSM303_H
