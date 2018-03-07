#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define IMU_ACC_ADDR 0x19
#define IMU_MAG_ADDR 0x1E

#define WHO_AM_I_ACC_RSP  0x68
#define WHO_AM_I_MAG_RSP  0x3D

#define IMU_PATH "/dev/i2c-1"
#define I2C_SLAVE 0x0703


/************************************************/
/*****         I2X DEVICE FUNCTIONS         *****/
/************************************************/
//I2C Read/Write Structs
union i2c_smbus_data
{
  uint8_t  byte ;
  uint16_t word ;
  uint8_t  block [34] ;	// block [0] is used for length + one more for PEC
} ;
/* Write a single byte to the I2C bus
 *  addr      :   device address
 *  sub_addr  :   register to be written
 *  val       :   value to be written in
 */
void i2c_write_byte(uint8_t addr, uint8_t sub_addr, uint8_t val){
  int fd = open(IMU_PATH, O_RDWR);
  ioctl(fd, I2C_SLAVE, addr);
  union i2c_smbus_data data;
  data.byte = val;
  struct i2c_smbus_ioctl_data io_data;
  io_data.read_write = 0;
  io_data.command = sub_addr;
  io_data.size = 2;
  io_data.data = &data;
  ioctl(fd, I2C_SMBUS, &io_data);
  close(fd);
}
/* Read a single byte from the I2C bus
 *  addr      :   device address
 *  sub_addr  :   register to be written
 */
uint8_t i2c_read_byte(uint8_t addr, uint8_t sub_addr){
    int fd = open("/dev/i2c-1", O_RDWR);
    ioctl(fd, I2C_SLAVE, addr);
    union i2c_smbus_data data;
    struct i2c_smbus_ioctl_data io_data;
    io_data.read_write = 1;
    io_data.command = sub_addr;
    io_data.size = 2;
    io_data.data = &data;
    ioctl(fd, I2C_SMBUS, &io_data);
    close(fd);

    return data.byte & 0xFF;
}

/************************************************/
/*****            ACCELEROMETER             *****/
/************************************************/
void init_acc(){
  // Select control register1(0x20)
	// X, Y and Z-axis enable, power on mode, o/p data rate 10 Hz(0x27)
	char config[2] = {0};
  i2c_write_byte(IMU_ACC_ADDR, 0x20, 0x27);
	// Select control register4(0x23)
	// Full scale +/- 2g, continuous update(0x00)
  i2c_write_byte(IMU_ACC_ADDR, 0x23, 0x00);
  //allow changes to take effect
	sleep(0.25);
}

uint16_t* read_acc(){
  //Acc data stored in regs 0x28:0x2D
  unsigned char data[6];
  data[0] = i2c_read_byte(IMU_ACC_ADDR, 0x28);
  data[1] = i2c_read_byte(IMU_ACC_ADDR, 0x29);  
  data[2] = i2c_read_byte(IMU_ACC_ADDR, 0x2A);
  data[3] = i2c_read_byte(IMU_ACC_ADDR, 0x2B);  
  data[4] = i2c_read_byte(IMU_ACC_ADDR, 0x2C);
  data[5] = i2c_read_byte(IMU_ACC_ADDR, 0x2D);

  //format into word
  uint16_t accel[3];
  accel[0] = data[1] * 256 + data[0];
  accel[1] = data[3] * 256 + data[2];
  accel[2] = data[5] * 256 + data[4];
  for(int i=0; i<3; i++)
    if(accel[i] > 32767)
      accel[i] -= 65536;

  return accel;
}

/************************************************/
/*****            MAGNETOMETER              *****/
/************************************************/
void init_mag(){
  // Select MR register(0x02)
  // Continuous conversion(0x00)
  i2c_write_byte(IMU_MAG_ADDR, 0x02, 0x00);
  // Select CRA register(0x00)
  // Data output rate = 15Hz(0x10)
  i2c_write_byte(IMU_ACC_ADDR, 0x00, 0x10);
  // Select CRB register(0x01)
  // Set gain = +/- 1.3g(0x20)
  i2c_write_byte(IMU_ACC_ADDR, 0x01, 0x20);
  sleep(0.25);
}

uint16_t* read_mag(){
  //Mag data stored in regs 0x03:0x08
  unsigned char data[6];
  data[0] = i2c_read_byte(IMU_MAG_ADDR, 0x03);
  data[1] = i2c_read_byte(IMU_MAG_ADDR, 0x04);  
  data[2] = i2c_read_byte(IMU_MAG_ADDR, 0x05);
  data[3] = i2c_read_byte(IMU_MAG_ADDR, 0x06);  
  data[4] = i2c_read_byte(IMU_MAG_ADDR, 0x07);
  data[5] = i2c_read_byte(IMU_MAG_ADDR, 0x08);

  //format into word
  uint16_t magnet[3];
  magnet[0] = data[1] * 256 + data[0];
  magnet[1] = data[3] * 256 + data[2];
  magnet[2] = data[5] * 256 + data[4];
  for(int i=0; i<3; i++)
    if(magnet[i] > 32767)
      magnet[i] -= 65536;

  return magnet;
}

/************************************************/
/*****            GENERAL IMU               *****/
/************************************************/
void init_imu(){
  init_acc();
  init_mag();
}