// Distributed with a free-will license
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works
// LSM303DLHC
// This code is designed to work with the LSM303DLHC_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/products

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "lsm303.h"

char* I2C_BUS = "/dev/i2c-1";
int I2C_PORT;

int main()
{
	// Open I2C bus
	I2C_PORT = open_I2C_port(I2C_BUS);
	if(I2C_PORT < 0)
	{
		fprintf(stderr, "Failed to open the bus. \n");
		return(1);
	}
	//config ACCELEROMETER
	config_accelerometer(I2C_PORT);

	// Read accelerometer data
	int x = get_accel_x(I2C_PORT);
	int y = get_accel_y(I2C_PORT);
	int z = get_accel_z(I2C_PORT);

	// Output data to screen
	printf("Acceleration: x y z\n");
	printf("%i %i %i\n", x, y, z);

	//config magnetometer
	config_magnetometer(I2C_PORT);

	// Read magnetometer data
	//in a loop
	printf("Magnetometer Bearing: x y z\n");

	int value_array[3][25];

	for(int count = 0; count < 25; count++)
	{
		value_array[0][count] = get_magnetom_x(I2C_PORT);
		value_array[1][count] = get_magnetom_y(I2C_PORT);
		value_array[2][count] = get_magnetom_z(I2C_PORT);
		printf("%i %i %i\n", value_array[0][count], value_array[1][count], value_array[2][count], z);
		config_magnetometer(I2C_PORT);
	}

	int x_ave = 0, y_ave = 0, z_ave = 0;
	for(int i=0; i<25; i++)
	{
		x_ave += value_array[0][i];
		y_ave += value_array[1][i];
		z_ave += value_array[2][i];
	}
	x_ave /= 25;
	y_ave /= 25;
	z_ave /= 25;
	int x_sd = 0, y_sd = 0, z_sd = 0;
	for(int j=0; j<25; j++)
	{
		x_sd += (value_array[0][j] - x_ave) * (value_array[0][j] - x_ave);
		y_sd += (value_array[1][j] - y_ave) * (value_array[1][j] - y_ave);
		y_sd += (value_array[2][j] - z_ave) * (value_array[2][j] - z_ave);
	}
	x_sd /= 24;
	y_sd /= 24;
	z_sd /= 24;

	printf("Array summaries:\n");
	printf("x_sum: %i +- %i\n", x_ave, x_sd);
	printf("y_sum: %i +- %i\n", y_ave, y_sd);
	printf("z_sum: %i +- %i\n", z_ave, z_sd);

	printf("Hope that was helpful!\n");
	return 0;
}
