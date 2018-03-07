#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define IMU_ACC_ADDR 0x19
#define IMU_MAG_ADDR 0x1E

#define WHO_AM_I_ACC_RSP  0x68
#define WHO_AM_I_MAG_RSP  0x3D

#define IMU_PATH "/dev/i2c-1"
#define I2C_SLAVE 0x0703

int ACCEL_RES = 1;
int MAG_RES = 1;

/************************************************/
/*****         I2C DEVICE FUNCTIONS         *****/
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
uint8_t i2c_read_byte(uint8_t addr, uint8_t sub_addr){\
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
  //Select control register1(0x20)
  //X, Y and Z-axis enable, power on mode, sample rate 100Hz(0x57)
  i2c_write_byte(IMU_ACC_ADDR, 0x20, 0x57);
  //Select control register4(0x23)
  //Full scale +/- 2g, continuous update, high resolution(0x08))
  i2c_write_byte(IMU_ACC_ADDR, 0x23, 0x08);
  //allow changes to take effect
  sleep(0.25);
}

int16_t* read_acc(){
  //Acc data stored in regs 0x28:0x2D
  unsigned char data[6];
  data[0] = i2c_read_byte(IMU_ACC_ADDR, 0x28);
  data[1] = i2c_read_byte(IMU_ACC_ADDR, 0x29);  
  data[2] = i2c_read_byte(IMU_ACC_ADDR, 0x2A);
  data[3] = i2c_read_byte(IMU_ACC_ADDR, 0x2B);  
  data[4] = i2c_read_byte(IMU_ACC_ADDR, 0x2C);
  data[5] = i2c_read_byte(IMU_ACC_ADDR, 0x2D);

  //format into word
  static int16_t accel[3];
  accel[0] = (data[1] << 8) | data[0];
  accel[1] = (data[3] << 8) | data[2];
  accel[2] = (data[5] << 8) | data[4];

  for(int i=0; i<3; i++)
    if(accel[i] > 32767)
      accel[i] -= 65536;

  return accel;
}

/************************************************/
/*****            MAGNETOMETER              *****/
/************************************************/
void init_mag(){
  //Set gain to +- 1.3 gauss
  i2c_write_byte(IMU_MAG_ADDR, 0x01, 0x020);
  //Select MR register(0x02)
  //Continuous conversion(0x00)
  i2c_write_byte(IMU_MAG_ADDR, 0x02, 0x00);
  //Select CRA register(0x00)
  //Data output rate = 15Hz(0x10)
  i2c_write_byte(IMU_ACC_ADDR, 0x00, 0x10);
  //Select CRB register(0x01)
  //Set gain = +/- 1.3g(0x20)
  i2c_write_byte(IMU_ACC_ADDR, 0x01, 0x20);
  sleep(0.25);
}

int16_t* read_mag(){
  //Mag data stored in regs 0x03:0x08
  unsigned char data[6];
  data[0] = i2c_read_byte(IMU_MAG_ADDR, 0x03);
  data[1] = i2c_read_byte(IMU_MAG_ADDR, 0x04);  
  data[2] = i2c_read_byte(IMU_MAG_ADDR, 0x05);
  data[3] = i2c_read_byte(IMU_MAG_ADDR, 0x06);  
  data[4] = i2c_read_byte(IMU_MAG_ADDR, 0x07);
  data[5] = i2c_read_byte(IMU_MAG_ADDR, 0x08);

  //format into word
  static int16_t magnet[3];
  magnet[0] = (data[0] << 8) | data[1];
  magnet[1] = (data[2] << 8) | data[3];
  magnet[2] = (data[4] << 8) | data[5];

  return magnet;
}

/************************************************/
/*****            GENERAL IMU               *****/
/************************************************/
void calibrate_imu(){
	//enable FIFO snd set length to 0x1F
	uint8_t tmp = i2c_read_byte(IMU_ACC_ADDR, 0x2E);
	tmp |= 0x40;
	i2c_write_byte(IMU_ACC_ADDR, 0x2E, tmp);

}

void init_imu(){
  init_acc();
  init_mag();
//  calibrate_imu();
}

int16_t* IMU_pull_samples(){
	static int16_t samples[6];
	int16_t* tmp;
	tmp = read_acc();
	samples[0] = tmp[0] * ACCEL_RES;
	samples[1] = tmp[1] * ACCEL_RES;
	samples[2] = tmp[2] * ACCEL_RES;

	tmp = read_mag();
	samples[0] = tmp[0] * MAG_RES;
	samples[1] = tmp[1] * MAG_RES;
	samples[2] = tmp[2] * MAG_RES;

	return samples;
}

void print_memory()
{
	printf("Accel:\n");
	for(int i=0; i<0x3F; i++){
		if((i%16) == 0) printf("\n");
		printf("%02x ", i2c_read_byte(IMU_ACC_ADDR, i));
	}
	printf("\nMag:\n");
	for(int i=0; i<0x3A; i++){
		if((i%16) == 0) printf("\n");
		printf("%02x ", i2c_read_byte(IMU_MAG_ADDR, i));
	}
	printf("\n");
}
