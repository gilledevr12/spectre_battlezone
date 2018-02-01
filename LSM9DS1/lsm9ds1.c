//LSM9DS1 Functions
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "lsm9ds1.h"
#include "LSM9DS1_Registers.h"

//Development Commands
//#define DEBUG

//IMU Declarations
#define IMU_XG_ADDR         0x6B
#define IMU_MAG_ADDR        0x1E
//Calibrated BIAS values for accuracy
int GYRO_BIAS[3] = {0, 0, 0};
int ACCEL_BIAS[3] = {0, 0, 0};
int MAG_BIAS[3] = {0, 0, 0};
//Gyroscope Declarations
#define GYRO_SCALE          245
#define GYRO_SAMPLE_RATE    6           //952
#define GYRO_BANDWIDTH      0
//Accelerometer Declarations
#define ACCEL_SCALE         2           
#define ACCEL_SAMPLE_RATE   6           //952 Hz
#define ACCEL_BANDWIDTH     -1          //determined by sample rate
//Magnetometer Declarations
#define MAG_SCALE           4
#define MAG_SAMPLE_RATE     7           //80 Hz
#define MAG_BANDWIDTH       0
#define MAG_TEMP_COMP       0           //temperature compensation disabled
#define MAG_PERFORMANCE     3           //ultra high power performance
#define MAG_MODE            0           //continuous conversion
//Some Res?
const float GYRO_RES    =   0.007476807;
const float ACCEL_RES   =   0.000061035;
const float MAG_RES     =   0.00014;

const char* I2C_PORT_NAME = "/dev/i2c-1";
int IMU_BUS;

/* GYROSCOPE Functions */
void init_gyro(){
    //set sample rate, scale, bandwidth
    unsigned char data[2] = {CTRL_REG1_G, 0xC0};
    write(IMU_BUS, data, 2);
    //setup not using interrupt
    data[0] = CTRL_REG2_G;
    data[1] =  0x00;
    write(IMU_BUS, data, 2);
    //set low power disable and HPF disable
    data[0] = CTRL_REG3_G;
    data[1] = 0x00;
    write(IMU_BUS, data, 2);
    //enable x y z axis, latched interrupt?
    data[0] = CTRL_REG4;
    data[1] = 0x39;
    write(IMU_BUS, data, 2);
    //configure orientation flip
    data[0] = ORIENT_CFG_G;
    data[1] = 0x00;
    write(IMU_BUS, data, 2);
    
    #ifdef DEBUG
        printf("Init GYRO complete\n");
    #endif
    
    return;
}

int* read_gyro(){
    unsigned char temp[6] = {0, 0, 0, 0, 0, 0};
    unsigned char addr[1] = {OUT_X_L_G};
    write(IMU_BUS, addr, 1);
    read(IMU_BUS, temp, 6);
    #ifdef DEBUG
        printf("Gyro received: ");
        for(int i=0; i<6; i++)
            printf("%02x ", temp[i]);
        printf("\n");
    #endif
    static int temp_gyro[3];
    temp_gyro[0] = ((temp[1] << 8) | temp[0]) - GYRO_BIAS[0];
    temp_gyro[1] = ((temp[3] << 8) | temp[2]) - GYRO_BIAS[1];
    temp_gyro[2] = ((temp[5] << 8) | temp[4]) - GYRO_BIAS[2];

    return temp_gyro;
}

/* ACCELEROMETER Functions */
void init_accel(){
    //Enable x, y, z axis
    unsigned char data[2] = {CTRL_REG5_XL, 0x38};
    write(IMU_BUS, data, 2);
    //Enable accel, set scale, bandwidth
    data[0] = CTRL_REG6_XL;
    data[1] =  0xC0;
    write(IMU_BUS, data, 2);
    //disable high resolution
    data[0] = CTRL_REG7_XL;
    data[1] = 0x00;
    write(IMU_BUS, data, 2);
    
    #ifdef DEBUG
        printf("Init ACCEL complete\n");
    #endif

    return;
}

int* read_accel(){
    unsigned char temp[6] = {0, 0, 0, 0, 0, 0};
    unsigned char addr[1] = {OUT_X_L_XL};
    write(IMU_BUS, addr, 1);
    read(IMU_BUS, temp, 6);
    #ifdef DEBUG
        printf("Accel received: ");
        for(int i=0; i<6; i++)
            printf("%02x ", temp[i]);
        printf("\n");
    #endif
    static int temp_accel[3];
    temp_accel[0] = ((temp[1] << 8) | temp[0]) - ACCEL_BIAS[0];
    temp_accel[1] = ((temp[3] << 8) | temp[2]) - ACCEL_BIAS[1];
    temp_accel[2] = ((temp[5] << 8) | temp[4]) - ACCEL_BIAS[2];

    return temp_accel;
}


/* MAGNETOMETER Functions */
void init_mag(){
    //Set xzy performance level and sample rate
    unsigned char data[2] = {CTRL_REG1_M, 0x7C};
    write(IMU_BUS, data, 2);
    //Set scale
    data[0] = CTRL_REG2_M;
    data[1] =  0x00;
    write(IMU_BUS, data, 2);
    //disable low power and continuous operation mode
    data[0] = CTRL_REG4_M;
    data[1] = 0x00;
    write(IMU_BUS, data, 2);
    //set z axis mode to ultra high performance
    data[0] = CTRL_REG5_M;
    data[1] = 0x0C;
    write(IMU_BUS, data, 2);

    #ifdef DEBUG
        printf("Init MAG complete\n");
    #endif

    return;
}

int* read_mag(){
    unsigned char temp[6] = {0, 0, 0, 0, 0, 0};
    unsigned char addr[1] = {OUT_X_L_M};
    write(IMU_BUS, addr, 1);
    read(IMU_BUS, temp, 6);
    #ifdef DEBUG
        printf("Mag received: ");
        for(int i=0; i<6; i++)
            printf("%02x ", temp[i]);
        printf("\n");
    #endif
    static int temp_mag[3];
    temp_mag[0] = ((temp[1] << 8) | temp[0]);
    temp_mag[1] = ((temp[3] << 8) | temp[2]);
    temp_mag[2] = ((temp[5] << 8) | temp[4]);

    return temp_mag;
}

/* IMU Functions */
unsigned char read_byte(unsigned char addr){
    unsigned char temp[1] = {addr};
    write(IMU_BUS, temp, 1);
   #ifdef DEBUG
        printf("IMU Read from %02x: ", temp[0]);
    #endif
    read(IMU_BUS, temp, 1);
    #ifdef DEBUG
        printf(" %02x\n", temp[0]);
    #endif
    return temp[0];
}

void write_byte(unsigned char addr, unsigned char val){
    unsigned char temp[2] = {addr, val};
    write(IMU_BUS, temp, 2);
    #ifdef DEBUG
        printf("IMU write %02x: %02x\n", temp[0], temp[1]);
    #endif
    return;
}

void calibrate_IMU(){
    printf("Calibration imcomplete. Proceed with caution...\n");

    //enable FIFO and set length 0x1F
    unsigned char temp = read_byte(0x23);
    temp |= 2;
    write_byte(0x23, temp);
    write_byte(0x2E, 0x3F);

    //pull back the number of stored sampled
    #ifdef DEBUG
        #undef DEBUG
        #define RECOVER_DEBUG
        printf("Pulling samples...");
    #endif
    unsigned char sample_count = 0;
    while(sample_count < 0x1F){
        sample_count = read_byte(0x2F);
        printf("Sample count: %02x %i\n", sample_count, sample_count);
    }
    #ifdef RECOVER_DEBUG
        #define DEBUG
        #undef RECOVER_DEBUG
        printf("done\n");
    #endif
    //pull samples
    int *temp_gyro, *temp_accel;
    int gyro_raw[3] = {0, 0, 0};
    int accel_raw[3] = {0, 0, 0};

    #ifdef DEBUG
        #undef DEBUG
        #define RECOVER_DEBUG
    #endif
    for(int i=0; i<sample_count; i++){
        temp_gyro = read_gyro();
        gyro_raw[0] += temp_gyro[0];
        gyro_raw[1] += temp_gyro[1];
        gyro_raw[2] += temp_gyro[2];

        temp_accel = read_accel();
        accel_raw[0] += temp_accel[0];
        accel_raw[1] += temp_accel[1];
        accel_raw[2] += temp_accel[2];
    }
    #ifdef RECOVER_DEBUG
        #define DEBUG
        #undef RECOVER_DEBUG
    #endif

    //disable FIFO and set length back to 0
    temp = read_byte(0x23);
    temp &= ~2;
    write_byte(0x23, temp);
    write_byte(0x2E, 0x00);

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
    IMU_BUS = open(I2C_PORT_NAME, O_RDWR);
    if(IMU_BUS < 0)
    {
        fprintf(stderr, "Failed to open the bus. \n");
        return(1);
    }

    //validate connection
    unsigned char READ_GX[1] = {0x0F}, READ_MAG[1] = {0x0F};
    unsigned char gx_resp[1] = {0}, mag_resp[1] = {0};

	//Read GYRO / ACCEL Response
    ioctl(IMU_BUS, I2C_SLAVE, IMU_XG_ADDR);
    write(IMU_BUS, READ_GX, 1);
    if(read(IMU_BUS, gx_resp, 1) != 1){
	    fprintf(stderr, "Error: I/O error on GX_RESP\n");
	    return 1;
    }
    #ifdef DEBUG
        printf("GX  response: %02x\t Expected response: %02x\n", gx_resp[0], 0x68);
    #endif

    //Read MAG Response
    ioctl(IMU_BUS, I2C_SLAVE, IMU_MAG_ADDR);
    write(IMU_BUS, READ_MAG, 1);
    if(read(IMU_BUS, mag_resp, 1) != 1){
	    fprintf(stderr, "Error: I/O error on MAG_RESP\n");
	    return 1;
    }
    #ifdef DEBUG
        printf("MAG response: %02x\t Expected response: %02x\n", mag_resp[0], 0x3d);
    #endif
    if((mag_resp[0] | gx_resp[0] << 8) != 0x683D) {
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
    static int samples[9];
    int *tmp;
    tmp = read_accel();
    samples[0] = tmp[0];
    samples[1] = tmp[1];
    samples[2] = tmp[2];
    tmp = read_gyro();
    samples[3] = tmp[0];
    samples[4] = tmp[1];
    samples[5] = tmp[2];
    tmp = read_mag();
    samples[6] = tmp[0];
    samples[7] = tmp[1];
    samples[8] = tmp[2];

    return samples;
}
