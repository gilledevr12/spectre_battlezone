//LSM9DS1 Functions
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#include "lsm9ds1.h"
#include "LSM9DS1_Registers.h"

//IMU Declarations
#define IMU_XG_ADDR         0x6B
#define IMU_MAG_ADDR        0x1E
//Calibrated BIAS values for accuracy
float GYRO_BIAS[3] = {0, 0, 0};
float ACCEL_BIAS[3] = {0, 0, 0};
float MAG_BIAS[3] = {0, 0, 0};
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
#define MAG_SCALE           16
#define MAG_SAMPLE_RATE     7           //80 Hz
#define MAG_BANDWIDTH       0
#define MAG_TEMP_COMP_EN    1           //temperature compensation disabled
#define MAG_PERFORMANCE     3           //ultra high power performance
#define MAG_MODE            0           //continuous conversion
#define MAG_CALIBRATION_COUNTER_MAX 3
#define MAG_X_OFFSET 	    372
#define MAG_Y_OFFSET	    670
//Some Res?
const float GYRO_RES    =   0.007476807;
const float ACCEL_RES   =   0.000061035;
const float MAG_RES     =   0.00014;

const char* I2C_PORT_NAME = "/dev/i2c-1";
int IMU_BUS;

/************************************************/
/*****         I2C DEVICE FUNCTIONS         *****/
/************************************************/
//I2C Read/Write Structs
union i2c_smbus_data{
  uint8_t  byte ;
  uint16_t word ;
  uint8_t  block [34] ;	// block [0] is used for length + one more for PEC
};

void read_device_bytes(unsigned char addr, unsigned char sub_addr, int16_t* dest){
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
    uint8_t temp_data[6];
    read(fd, temp_data, 6);

    //close the device
    close(fd);

    //combine to integer values
    int16_t compiled_data[3];
    if(addr == IMU_XG_ADDR){
        compiled_data[0] = (temp_data[0] << 8) | temp_data[1];
        compiled_data[1] = (temp_data[2] << 8) | temp_data[3];
        compiled_data[2] = (temp_data[4] << 8) | temp_data[5];
    }
    else{
        compiled_data[0] = (temp_data[1] << 8) | temp_data[0];
        compiled_data[1] = (temp_data[3] << 8) | temp_data[2];
        compiled_data[2] = (temp_data[5] << 8) | temp_data[4];
    }
    
    //determine type, apply bias to result
    if((sub_addr == OUT_X_L_XL) && (addr == IMU_XG_ADDR)){     //Accelerometer Read
        compiled_data[0] -= ACCEL_BIAS[0];
        compiled_data[1] -= ACCEL_BIAS[1];
        compiled_data[2] -= ACCEL_BIAS[2];
    }
    else if((sub_addr == OUT_X_L_G) && (addr == IMU_XG_ADDR)){ //Gyro Read
        compiled_data[0] -= GYRO_BIAS[0];
        compiled_data[1] -= GYRO_BIAS[1];
        compiled_data[2] -= GYRO_BIAS[2];
    }
    else{                           //Mag read
        compiled_data[0] = compiled_data[0] - MAG_BIAS[0] - MAG_X_OFFSET;
        compiled_data[1] = compiled_data[1] - MAG_BIAS[1] - MAG_Y_OFFSET;
        compiled_data[2] -= MAG_BIAS[2];
    }

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

/************************************************/
/*****            ACCELEROMETER             *****/
/************************************************/
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

/************************************************/
/*****              GYROSCOPE               *****/
/************************************************/
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

/************************************************/
/*****            MAGNETOMETER              *****/
/************************************************/
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
    imu_write_byte(IMU_MAG_ADDR, CTRL_REG3_M, data);

    //set z axis mode to ultra high performance
    data = (MAG_PERFORMANCE & 3) << 2;
    imu_write_byte(IMU_MAG_ADDR, CTRL_REG4_M, data);

    //block domain update disable
    data = 0x40;
    imu_write_byte(IMU_MAG_ADDR, CTRL_REG5_M, data);

    //disable interrupt generation for X-Y-Z
    data = 0x00;
    imu_write_byte(IMU_MAG_ADDR, INT_CFG_M, data);

    #ifdef DEBUG
        printf("Init MAG complete\n");
    #endif

    return;
}

void calibrate_mag(){
        unsigned char dev_ready = 0;
        short tmp_read[3];
        read_device_bytes(IMU_MAG_ADDR, OUT_X_L_M, tmp_read);
        int16_t mag_min[3] = {tmp_read[0], tmp_read[1], tmp_read[2]};
        int16_t mag_max[3] = {tmp_read[0], tmp_read[1], tmp_read[2]};
        //iterate some number of times to get a valid average value for x y z
        for(int c=0; c<MAG_CALIBRATION_COUNTER_MAX; c++){
	    dev_ready = 0;
            while(!dev_ready){
	        dev_ready = (imu_read_byte(IMU_MAG_ADDR, STATUS_REG_M) & 8) >> 3;
	    }
            read_device_bytes(IMU_MAG_ADDR, OUT_X_L_M, tmp_read);
            //adjust max and min for newly read values
            for(int i=0; i<3; i++){
                if(tmp_read[i] > mag_max[i])
                    mag_max[i] = tmp_read[i];            
                if(tmp_read[i] < mag_min[i])
                    mag_min[i] = tmp_read[i];
            }
        }
        //adjust into the bias variable
        for (int j = 0; j < 3; j++){
            MAG_BIAS[j] = ((mag_max[j] + mag_min[j]) / 2) * MAG_RES;
            // if (loadIn)
            //     magOffset(j, mBiasRaw[j]);
        }
}

/************************************************/
/*****            GENERAL IMU               *****/
/************************************************/
void calibrate_IMU(){
    //enable FIFO and set length 0x1F
    unsigned char temp = imu_read_byte(IMU_XG_ADDR, CTRL_REG9);
    temp |= 2;
    imu_write_byte(IMU_XG_ADDR, CTRL_REG9, temp);
    imu_write_byte(IMU_XG_ADDR, FIFO_CTRL, 0x3F);

    //pull back the number of stored sampled
    unsigned char sample_count = 0;
    while(sample_count < 0x1F){
            sample_count = imu_read_byte(IMU_XG_ADDR, FIFO_SRC) & 0x1F;
    }
    //pull samples
    short temp_gyro[3], temp_accel[3];
    short gyro_raw[3] = {0, 0, 0};
    short accel_raw[3] = {0, 0, 0};

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

    //disable FIFO and set length back to 0
    temp = imu_read_byte(IMU_XG_ADDR, CTRL_REG9);
    temp &= ~2;
    imu_write_byte(IMU_XG_ADDR, CTRL_REG9, temp);
    imu_write_byte(IMU_XG_ADDR, FIFO_CTRL, 0x00);

    //calculate bias from samples
    GYRO_BIAS[0] = GYRO_RES * (gyro_raw[0] / sample_count);
    GYRO_BIAS[1] = GYRO_RES * (gyro_raw[1] / sample_count);
    GYRO_BIAS[2] = GYRO_RES * (gyro_raw[2] / sample_count);

    #ifdef DEBUG
            printf("Calibrated GYRO: %2.4f %2.4f %2.4f\n",
                    GYRO_BIAS[0], GYRO_BIAS[1], GYRO_BIAS[2]);
    #endif

    ACCEL_BIAS[0] = ACCEL_RES * (accel_raw[0] / sample_count);
    ACCEL_BIAS[1] = ACCEL_RES * (accel_raw[1] / sample_count);
    ACCEL_BIAS[2] = ACCEL_RES * (accel_raw[2] / sample_count);

    #ifdef DEBUG
            printf("Calibrated ACCL: %2.4f %2.4f %2.4f\n",
                    ACCEL_BIAS[0], ACCEL_BIAS[1], ACCEL_BIAS[2]);
    #endif

    calibrate_mag();

     #ifdef DEBUG
            printf("Calibrated MAG: %2.4f %2.4f %2.4f\n",
                    MAG_BIAS[0], MAG_BIAS[1], MAG_BIAS[2]);
    #endif
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
        printf(" XG response: %02x\t Expected response: %02x\n", xg_resp, 0x68);
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

    //reset the ACC/GYRO and MAG
    //imu_write_byte(IMU_XG_ADDR, CTRL_REG8, 0x05);
    imu_write_byte(IMU_MAG_ADDR, CTRL_REG2_M, 0x04);

    sleep(0.1);

    init_gyro();
    init_accel();
    init_mag();
    
    //Calibrate the IMU Device
    calibrate_IMU();

    return 0;
}

float* IMU_pull_samples(){
    //sample read is defined int s[9]: {a1, a2, a3 g1, g2, g3, m1, m2, m3
    static float samples[9];
    int16_t tmp[3] = {0, 0, 0};
    //read accelerometer
    unsigned char dev_ready = 0;
    while(!dev_ready)
        dev_ready = imu_read_byte(IMU_XG_ADDR, STATUS_REG_1) & 1;
    read_device_bytes(IMU_XG_ADDR, OUT_X_L_XL, tmp);
    samples[0] = tmp[0] * ACCEL_RES;
    samples[1] = tmp[1] * ACCEL_RES;
    samples[2] = tmp[2] * ACCEL_RES;
    //read gyroscope
    dev_ready = 0;
    while(!dev_ready)
        dev_ready = (imu_read_byte(IMU_XG_ADDR, STATUS_REG_1) & 2) >> 1;
    read_device_bytes(IMU_XG_ADDR, OUT_X_L_G, tmp);
    samples[3] = tmp[0] * GYRO_RES;
    samples[4] = tmp[1] * GYRO_RES;
    samples[5] = tmp[2] * GYRO_RES;
    //read magnetometer
    dev_ready = 0;
    while(!dev_ready)
        dev_ready = (imu_read_byte(IMU_MAG_ADDR, STATUS_REG_M) & 8) >> 3;
    read_device_bytes(IMU_MAG_ADDR, OUT_X_L_M, tmp);
    samples[6] = tmp[0] * MAG_RES;
    samples[7] = tmp[1] * MAG_RES;
    samples[8] = tmp[2] * MAG_RES;

    return samples;
}

int16_t* IMU_pull_samples_int(){
    //sample read is defined int s[9]: {a1, a2, a3 g1, g2, g3, m1, m2, m3
    //currently only reading ACC and MAG, GYRO is commented out

    static int16_t samples[6];
    int16_t tmp[3] = {0, 0, 0};
    //read accelerometer
    unsigned char dev_ready = 0;
    while(!dev_ready)
        dev_ready = imu_read_byte(IMU_XG_ADDR, STATUS_REG_1) & 1;
    read_device_bytes(IMU_XG_ADDR, OUT_X_L_XL, tmp);
    samples[0] = tmp[0];// * ACCEL_RES;
    samples[1] = tmp[1];// * ACCEL_RES;
    samples[2] = tmp[2];// * ACCEL_RES;
    //read gyroscope
    // dev_ready = 0;
    // while(!dev_ready)
    //     dev_ready = (imu_read_byte(IMU_XG_ADDR, STATUS_REG_1) & 2) >> 1;
    // read_device_bytes(IMU_XG_ADDR, OUT_X_L_G, tmp);
    // samples[3] = tmp[0];// * GYRO_RES;
    // samples[4] = tmp[1];// * GYRO_RES;
    // samples[5] = tmp[2];// * GYRO_RES;
    //read magnetometer
    dev_ready = 0;
    while(!dev_ready)
        dev_ready = (imu_read_byte(IMU_MAG_ADDR, STATUS_REG_M) & 8) >> 3;
    read_device_bytes(IMU_MAG_ADDR, OUT_X_L_M, tmp);
    samples[3] = tmp[0];// * MAG_RES;
    samples[4] = tmp[1];// * MAG_RES;
    samples[5] = tmp[2];// * MAG_RES;

    return samples;
}

void read_memory(){
    //Read XG Memory
    printf("XG Memory:\n");
    for(int i=0; i<0x36; i=i+4){
        printf("%02x: %02x %02x %02x %02x\n",
            i, imu_read_byte(IMU_XG_ADDR, i), imu_read_byte(IMU_XG_ADDR, i+1),
            imu_read_byte(IMU_XG_ADDR, i+2), imu_read_byte(IMU_XG_ADDR, i+3));
    }
    printf("\n\n");

    //Read XG Memory
    printf("Mag Memory:\n");
    for(int i=0; i<0x32; i=i+4){
        printf("%02x: %02x %02x %02x %02x\n",
            i, (int) imu_read_byte(IMU_MAG_ADDR, i)  , (int) imu_read_byte(IMU_MAG_ADDR, i+1),
               (int) imu_read_byte(IMU_MAG_ADDR, i+2), (int) imu_read_byte(IMU_MAG_ADDR, i+3));
    }
    printf("\n\n");
}
