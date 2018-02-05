//LSM9DS1 Functions
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "lsm9ds1.h"
#include "LSM9DS1_Registers.h"

//Development Commands
#define DEBUG

//IMU Declarations
#define IMU_XG_ADDR         0x6B
#define IMU_MAG_ADDR        0x1E
//Calibrated BIAS values for accuracy
int GYRO_BIAS[3] = {0, 0, 0};
int ACCEL_BIAS[3] = {0, 0, 0};
int MAG_BIAS[3] = {0, 0, 0};
//I2C Declarations
#define I2C_SLAVE 0x0703

//Gyroscope Declarations
#define GYRO_SAMPLE_RATE    6           //952
#define GYRO_SCALE          245
#define GYRO_BANDWIDTH      0
//Accelerometer Declarations
#define ACCEL_SCALE         2           
#define ACCEL_SAMPLE_RATE   6           //952 Hz
#define ACCEL_BANDWIDTH     -1          //determined by sample rate
//Magnetometer Declarations
#define MAG_SCALE           4
#define MAG_SAMPLE_RATE     7           //80 Hz
#define MAG_BANDWIDTH       0
#define MAG_TEMP_COMP_EN    0           //temperature compensation disabled
#define MAG_PERFORMANCE     3           //ultra high power performance
#define MAG_MODE            0           //continuous conversion
//Some Res?
const float GYRO_RES    =   0.007476807;
const float ACCEL_RES   =   0.000061035;
const float MAG_RES     =   0.00014;

const char* I2C_PORT_NAME = "/dev/i2c-1";
int IMU_BUS;

//I2C Read/Write Structs
union i2c_smbus_data
{
  uint8_t  byte ;
  uint16_t word ;
  uint8_t  block [34] ;	// block [0] is used for length + one more for PEC
} ;

/* GYROSCOPE Functions */
void init_gyro(){
    //set sample rate, scale, bandwidth
    unsigned char data = 0;
    data = ((GYRO_SAMPLE_RATE & 0x07) << 5);
    if(GYRO_SCALE == 500)
        data |= (1 << 3);
    else if(GYRO_SCALE == 2000)
        data |= (3 << 3);
    else
        data = data;
    data |= (GYRO_BANDWIDTH & 3);
    imu_write_byte(IMU_XG_ADDR, CTRL_REG1_G, data);

    //setup not using interrupt
    data =  0x00;
    imu_write_byte(IMU_XG_ADDR, CTRL_REG2_G, data);

    //set low power disable and HPF disable
    data = 0x00;
    imu_write_byte(IMU_XG_ADDR, CTRL_REG3_G, data);

    //enable x y z axis, latched interrupt = true
    data = 0x3A;
    imu_write_byte(IMU_XG_ADDR,CTRL_REG4, data);

    //configure orientation flip
    data = 0x00;
    imu_write_byte(IMU_XG_ADDR, ORIENT_CFG_G, data);
    
    #ifdef DEBUG
        printf("Init GYRO complete\n");
    #endif
    
    return;
}

// int* read_gyro(){
//     unsigned char temp[6] = {0, 0, 0, 0, 0, 0};
//     unsigned char addr[1] = {OUT_X_L_G};
//     write(IMU_BUS, addr, 1);
//     read(IMU_BUS, temp, 6);
//     #ifdef DEBUG
//         printf("Gyro received: ");
//         for(int i=0; i<6; i++)
//             printf("%02x ", temp[i]);
//         printf("\n");
//     #endif
//     static int temp_gyro[3];
//     temp_gyro[0] = ((temp[1] << 8) | temp[0]) - GYRO_BIAS[0];
//     temp_gyro[1] = ((temp[3] << 8) | temp[2]) - GYRO_BIAS[1];
//     temp_gyro[2] = ((temp[5] << 8) | temp[4]) - GYRO_BIAS[2];

//     return temp_gyro;
// }

/* ACCELEROMETER Functions */
void init_accel(){
    //Enable x, y, z axis
    unsigned char data = 0x38;
    imu_write_byte(IMU_XG_ADDR, CTRL_REG5_XL, data);

    //Enable accel, set scale, bandwidth
    data = (ACCEL_SAMPLE_RATE & 0x7) << 5;
    if(ACCEL_SCALE == 4)
        data |= (2 << 3);
    else if(ACCEL_SCALE == 8)
        data |= (3 << 3);
    else if(ACCEL_SCALE == 16)
        data |= (1 << 3);
    else
        data = data;
    if(ACCEL_BANDWIDTH >= 0){
        data |= 1 << 2;
        data |= (ACCEL_BANDWIDTH & 0x3);
    }
    imu_write_byte(IMU_XG_ADDR, CTRL_REG6_XL, data);

    //disable high resolution
    data = 0x00;
    imu_write_byte(IMU_XG_ADDR, CTRL_REG7_XL, data);
    
    #ifdef DEBUG
        printf("Init ACCEL complete\n");
    #endif

    return;
}

// int* read_accel(){
//     unsigned char temp[6] = {0, 0, 0, 0, 0, 0};
//     unsigned char addr[1] = {OUT_X_L_XL};
//     write(IMU_BUS, addr, 1);
//     read(IMU_BUS, temp, 6);
//     #ifdef DEBUG
//         printf("Accel received: ");
//         for(int i=0; i<6; i++)
//             printf("%02x ", temp[i]);
//         printf("\n");
//     #endif
//     static int temp_accel[3];
//     temp_accel[0] = ((temp[1] << 8) | temp[0]) - ACCEL_BIAS[0];
//     temp_accel[1] = ((temp[3] << 8) | temp[2]) - ACCEL_BIAS[1];
//     temp_accel[2] = ((temp[5] << 8) | temp[4]) - ACCEL_BIAS[2];

//     return temp_accel;
// }

/* MAGNETOMETER Functions */
void init_mag(){
    //Set xzy performance level and sample rate
    unsigned char data = 0x00;
    if(MAG_TEMP_COMP_EN)
        data |= (1 << 7);
    data |= ((MAG_PERFORMANCE & 3) << 5) | ((MAG_SAMPLE_RATE & 7) << 2);
    imu_write_byte(IMU_MAG_ADDR, CTRL_REG1_M, data);

    //Set scale
    if(MAG_SCALE == 8)
        data = (1 << 5);
    else if(MAG_SCALE == 12)
        data = (2 << 5);
    else if(MAG_SCALE == 16)
        data = (3 << 5);
    else
        data = 0;
    imu_write_byte(IMU_MAG_ADDR, CTRL_REG2_M, data);

    //disable low power and continuous operation mode
    data = 0x00;
    imu_write_byte(IMU_MAG_ADDR, CTRL_REG4_M, data);

    //set z axis mode to ultra high performance
    data = (MAG_PERFORMANCE & 3) << 2;
    imu_write_byte(IMU_MAG_ADDR, CTRL_REG5_M, data);

    #ifdef DEBUG
        printf("Init MAG complete\n");
    #endif

    return;
}

// int* read_mag(){
//     unsigned char temp[6] = {0, 0, 0, 0, 0, 0};
//     unsigned char addr[1] = {OUT_X_L_M};
//     write(IMU_BUS, addr, 1);
//     read(IMU_BUS, temp, 6);
//     #ifdef DEBUG
//         printf("Mag received: ");
//         for(int i=0; i<6; i++)
//             printf("%02x ", temp[i]);
//         printf("\n");
//     #endif
//     static int temp_mag[3];
//     temp_mag[0] = ((temp[1] << 8) | temp[0]);
//     temp_mag[1] = ((temp[3] << 8) | temp[2]);
//     temp_mag[2] = ((temp[5] << 8) | temp[4]);

//     return temp_mag;
// }

/* IMU Functions */
void read_device_bytes(unsigned char addr, unsigned char sub_addr, unsigned int* dest){
    int fd = open("/dev/i2c-1", O_RDWR);
    ioctl(fd, I2C_SLAVE, addr);

    //probe the device to send back the device data
    struct i2c_smbus_ioctl_data io_data;
    io_data.read_write = 1;
    io_data.command = sub_addr;
    io_data.size = 2;
    io_data.data = NULL;
    ioctl (fd, I2C_SMBUS, &io_data) ;

    //read back 6 bytes from device
    unsigned char temp_data[6];
    read(fd, temp_data, 6);

    //close the device
    close(fd);

    //combine to integer values
    unsigned int compiled_data[3];
    compiled_data[0] = (temp_data[1] << 8) | temp_data[0];
    compiled_data[1] = (temp_data[3] << 8) | temp_data[2];
    compiled_data[2] = (temp_data[5] << 8) | temp_data[4];

    //determine type, apply bias to result
    if(sub_addr == OUT_X_L_XL){ //Accelerometer Read
        compiled_data[0] -= ACCEL_BIAS[0];
        compiled_data[1] -= ACCEL_BIAS[1];
        compiled_data[2] -= ACCEL_BIAS[2];
    }
    else if(sub_addr == OUT_X_L_G){ //Gyro Read
        compiled_data[0] -= GYRO_BIAS[0];
        compiled_data[1] -= GYRO_BIAS[1];
        compiled_data[2] -= GYRO_BIAS[2];
    }
    else                            //Mag read - do nothing to adjust
        compiled_data[0] = compiled_data[0];

    //return compiled result
    dest[0] = compiled_data[0];
    dest[1] = compiled_data[1];
    dest[2] = compiled_data[2];

  return;
}

unsigned char imu_read_byte(unsigned char addr, unsigned char sub_addr){
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
    /*
    unsigned char temp[1] = {addr};
    write(IMU_BUS, temp, 1);
   #ifdef DEBUG
        printf("IMU Read from %02x: ", temp[0]);
    #endif
    read(IMU_BUS, temp, 1);
    */
    #ifdef DEBUG_VERBOSE
        if(addr == IMU_XG_ADDR)
            printf("Reading XG ");
        else
            printf("Reading MAG ");
        printf("reg %02x: %02x\n", sub_addr, data.byte & 0xFF);
    #endif

    return data.byte & 0xFF;
}

//write byte to i2c: device addr, reg addr, value to be written
void imu_write_byte(unsigned char addr, unsigned char sub_addr, unsigned char val){
    int fd = open("/dev/i2c-1", O_RDWR);
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

    // unsigned char temp[2] = {addr, val};
    // write(IMU_BUS, temp, 2);
    #ifdef DEBUG_VERBOSE
        if(addr == IMU_XG_ADDR)
            printf("Writing XG ");
        else
            printf("Writing MAG ");
        printf("reg %02x with value %02x\n", sub_addr, val);
    #endif
    
    return;
}

void calibrate_IMU(){
    printf("Calibration imcomplete. Proceed with caution...\n");

    //enable FIFO and set length 0x1F
    unsigned char temp = imu_read_byte(IMU_XG_ADDR, CTRL_REG9);
    temp |= 2;
    imu_write_byte(IMU_XG_ADDR, CTRL_REG9, temp);
    imu_write_byte(IMU_XG_ADDR, FIFO_CTRL, 0x3F);

    //pull back the number of stored sampled
    #ifdef DEBUG
        #undef DEBUG
        #define RECOVER_DEBUG
        printf("Pulling samples...");
    #endif
    unsigned char sample_count = 0;
    while(sample_count < 0x1F){
            sample_count = imu_read_byte(IMU_XG_ADDR, FIFO_SRC);
        printf("Sample count: %02x %i\n", sample_count, sample_count);
    }
    #ifdef RECOVER_DEBUG
        #define DEBUG
        #undef RECOVER_DEBUG
        printf("done\n");
    #endif
    //pull samples
    int temp_gyro[3], temp_accel[3];
    int gyro_raw[3] = {0, 0, 0};
    int accel_raw[3] = {0, 0, 0};

    #ifdef DEBUG
        #undef DEBUG
        #define RECOVER_DEBUG
    #endif
    for(int i=0; i<sample_count; i++){
        read_device_bytes(IMU_XG_ADDR, OUT_X_L_G, temp_gyro);
        gyro_raw[0] += temp_gyro[0];
        gyro_raw[1] += temp_gyro[1];
        gyro_raw[2] += temp_gyro[2];

        read_device_bytes(IMU_XG_ADDR, OUT_X_L_XL, temp_accel);
        accel_raw[0] += temp_accel[0];
        accel_raw[1] += temp_accel[1];
        accel_raw[2] += temp_accel[2];
    }
    #ifdef RECOVER_DEBUG
        #define DEBUG
        #undef RECOVER_DEBUG
    #endif

    //disable FIFO and set length back to 0
    temp = imu_read_byte(IMU_XG_ADDR, CTRL_REG9);
    temp &= ~2;
    imu_write_byte(IMU_XG_ADDR, CTRL_REG9, temp);
    imu_write_byte(IMU_XG_ADDR, FIFO_CTRL, 0x00);

    //calculate bias from samples
    GYRO_BIAS[0] = GYRO_RES * (gyro_raw[0] / sample_count);
    GYRO_BIAS[1] = GYRO_RES * (gyro_raw[1] / sample_count);
    GYRO_BIAS[2] = GYRO_RES * (gyro_raw[2] / sample_count);

    ACCEL_BIAS[0] = ACCEL_RES * (accel_raw[0] / sample_count);
    ACCEL_BIAS[1] = ACCEL_RES * (accel_raw[1] / sample_count);
    ACCEL_BIAS[2] = ACCEL_RES * (accel_raw[2] / sample_count);
}

char init_IMU(){
    //Open I2C Port
    //this was moved to the read/write function calls
    // IMU_BUS = open(I2C_PORT_NAME, O_RDWR);
    // if(IMU_BUS < 0)
    // {
    //     fprintf(stderr, "Failed to open the bus. \n");
    //     return(1);
    // }

    //validate connection
    unsigned char READ_XG = 0x0F, READ_MAG = 0x0F;
    unsigned char xg_resp = 0, mag_resp = {0};

	//Read GYRO / ACCEL Response
    // ioctl(IMU_BUS, I2C_SLAVE, IMU_XG_ADDR);
    //write(IMU_BUS, READ_GX, 1);
    // if(read(IMU_BUS, gx_resp, 1) != 1){
	//     fprintf(stderr, "Error: I/O error on GX_RESP\n");
	//     return 1;
    // }
    xg_resp = imu_read_byte(IMU_XG_ADDR, READ_XG);
    #ifdef DEBUG
        printf("XG response: %02x\t Expected response: %02x\n", xg_resp, 0x68);
    #endif

    //Read MAG Response
    // ioctl(IMU_BUS, I2C_SLAVE, IMU_MAG_ADDR);
    mag_resp = imu_read_byte(IMU_MAG_ADDR, READ_MAG);
    // if(read(IMU_BUS, mag_resp, 1) != 1){
	//     fprintf(stderr, "Error: I/O error on MAG_RESP\n");
	//     return 1;
    // }
    #ifdef DEBUG
        printf("MAG response: %02x\t Expected response: %02x\n", mag_resp, 0x3d);
    #endif
    if((mag_resp | xg_resp << 8) != 0x683D) {
        fprintf(stderr, "Could not establish connection with LSM9DS1. Quitting\n");
        return 1;
    }

    #ifdef DEBUG
    	printf("LSM9DS1 connection established!\n");
    #endif

    init_gyro();
    init_accel();
    init_mag();
    
    //Calibrate the IMU Device
    //calibrate_IMU();

    return 0;
}

int* IMU_pull_samples(){
    //sample read is defined int s[9]: {a1, a2, a3 g1, g2, g3, m1, m2, m3
    static int samples[9];
    int tmp[3] = {0, 0, 0};
    //read accelerometer
    read_device_bytes(IMU_XG_ADDR, OUT_X_L_XL, tmp);
    samples[0] = tmp[0];
    samples[1] = tmp[1];
    samples[2] = tmp[2];
    read_device_bytes(IMU_XG_ADDR, OUT_X_L_G, tmp);
    samples[3] = tmp[0];
    samples[4] = tmp[1];
    samples[5] = tmp[2];
    read_device_bytes(IMU_MAG_ADDR, OUT_X_L_M, tmp);
    samples[6] = tmp[0];
    samples[7] = tmp[1];
    samples[8] = tmp[2];

    return samples;
}
