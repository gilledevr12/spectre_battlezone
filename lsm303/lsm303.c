#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define ACCELERO_SLAVE_ADDR 0x19
#define MAGNETOM_SLAVE_ADDR 0x1E

#define WHO_AM_I_AG_RSP   0x68
#define WHO_AM_I_M_RSP    0x3D

int open_I2C_port(char* path){
  return open(path, O_RDWR);
}

/************************************************/
/*****            ACCELEROMETER             *****/
/************************************************/
void config_accelerometer(int bus){
  //set slave address to accelerometer
  ioctl(bus, I2C_SLAVE, ACCELERO_SLAVE_ADDR);

  // Select control register1(0x20)
	// X, Y and Z-axis enable, power on mode, o/p data rate 10 Hz(0x27)
	char config[2] = {0};
	config[0] = 0x20;
	config[1] = 0x27;
	write(bus, config, 2);
	// Select control register4(0x23)
	// Full scale +/- 2g, continuous update(0x00)
	config[0] = 0x23;
	config[1] = 0x00;
	write(bus, config, 2);
	sleep(1);
}

int get_accel_x(int bus){
  //accelerometer stores 2B of data LSB first
  ioctl(bus, I2C_SLAVE, ACCELERO_SLAVE_ADDR);
  // Read xAccl lsb data from register(0x28)
  char reg[1] = {0x28};
  write(bus, reg, 1);
  char data[1] = {0};
  if(read(bus, data, 1) != 1)
  {
    fprintf(stderr, "Erorr : Input/output Erorr \n");
    return(1);
  }
  char data_a = data[0];

  // Read xAccl msb data from register(0x29)
  reg[0] = 0x29;
  write(bus, reg, 1);
  read(bus, data, 1);
  char data_b = data[0];

  //format into word
  int accel = data_b * 256 + data_a;
  if(accel > 32767)
    accel -= 65536;

  return accel;
}

int get_accel_y(int bus){
  //accelerometer stores 2B of data LSB first
  ioctl(bus, I2C_SLAVE, ACCELERO_SLAVE_ADDR);
  // Read yAccl lsb data from register(0x2A)
  char reg[1] = {0x2A};
  write(bus, reg, 1);
  char data[1] = {0};
  if(read(bus, data, 1) != 1)
  {
    fprintf(stderr, "Erorr : Input/output Erorr \n");
    return(1);
  }
  char data_a = data[0];

  // Read yAccl msb data from register(0x2B)
  reg[0] = 0x2B;
  write(bus, reg, 1);
  read(bus, data, 1);
  char data_b = data[0];

  //format into word
  int accel = data_b * 256 + data_a;
  if(accel > 32767)
    accel -= 65536;

  return accel;
}

int get_accel_z(int bus){
  //accelerometer stores 2B of data LSB first
  ioctl(bus, I2C_SLAVE, ACCELERO_SLAVE_ADDR);
  // Read zAccl lsb data from register(0x2C)
  char reg[1] = {0x2C};
  write(bus, reg, 1);
  char data[1] = {0};
  if(read(bus, data, 1) != 1)
  {
    fprintf(stderr, "Error : Input/output Erorr \n");
    return(1);
  }
  char data_a = data[0];

  // Read yAccl msb data from register(0x2D)
  reg[0] = 0x2D;
  write(bus, reg, 1);
  read(bus, data, 1);
  char data_b = data[0];

  //format into word
  int accel = data_b * 256 + data_a;
  if(accel > 32767)
    accel -= 65536;

  return accel;
}

/************************************************/
/*****            MAGNETOMETER              *****/
/************************************************/
void config_magnetometer(int bus){
  // Get I2C device, LSM303DLHC MAGNETO I2C address is 0x1E(30)
  ioctl(bus, I2C_SLAVE, MAGNETOM_SLAVE_ADDR);

  // Select MR register(0x02)
  // Continuous conversion(0x00)
  char config[2];
  config[0] = 0x02;
  config[1] = 0x00;
  write(bus, config, 2);
  // Select CRA register(0x00)
  // Data output rate = 15Hz(0x10)
  config[0] = 0x00;
  config[1] = 0x10;
  write(bus, config, 2);
  // Select CRB register(0x01)
  // Set gain = +/- 1.3g(0x20)
  config[0] = 0x01;
  config[1] = 0x20;
  write(bus, config, 2);
  sleep(1);
}

int get_magnetom_x(int bus){
  //magnetometer stores 2B of data MSB first
  ioctl(bus, I2C_SLAVE, MAGNETOM_SLAVE_ADDR);
  // Read xMag msb data from register(0x03)
	char reg[1] = {0x03};
  char data[1];
	write(bus, reg, 1);
	read(bus, data, 1);
	char data_a = data[0];

	// Read xMag lsb data from register(0x04)
	reg[0] = 0x04;
	write(bus, reg, 1);
	read(bus, data, 1);
	char data_b = data[0];

  int xMag = (data_a * 256 + data_b);
  if(xMag > 32767)
  {
    xMag -= 65536;
  }

  return xMag;
}

int get_magnetom_y(int bus){
  //magnetometer stores 2B of data MSB first
  ioctl(bus, I2C_SLAVE, MAGNETOM_SLAVE_ADDR);
  // Read yMag msb data from register(0x05)
	char reg[1] = {0x05};
  char data[1];
	write(bus, reg, 1);
	read(bus, data, 1);
	char data_a = data[0];

	// Read yMag lsb data from register(0x06)
	reg[0] = 0x06;
	write(bus, reg, 1);
	read(bus, data, 1);
	char data_b = data[0];

  int yMag = (data_a * 256 + data_b) ;
  if(yMag > 32767)
  {
    yMag -= 65536;
  }

  return yMag;
}

int get_magnetom_z(int bus){
  //magnetometer stores 2B of data MSB first
  ioctl(bus, I2C_SLAVE, MAGNETOM_SLAVE_ADDR);
  // Read zMag msb data from register(0x07)
	char reg[1] = {0x07};
  char data[1];
	write(bus, reg, 1);
	read(bus, data, 1);
	char data_a = data[0];

	// Read zMag lsb data from register(0x08)
	reg[0] = 0x08;
	write(bus, reg, 1);
	read(bus, data, 1);
	char data_b = data[0];

  int zMag = (data_a * 256 + data_b) ;
  if(zMag > 32767)
  {
    zMag -= 65536;
  }

  return zMag;
}
