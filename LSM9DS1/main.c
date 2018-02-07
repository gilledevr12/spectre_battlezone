#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <time.h>
#include "lsm9ds1.h"
#include <math.h>

int main(){
	init_IMU();
	calibrate_IMU();

	//printf("Polling samples: \tAccel (Gs) \t\t\tGyro (deg/s) \t\t\t\tMag(gauss)\n");

	#define loop_count 25
	float arr[9][loop_count];
	for(int i=0; i<loop_count; i++){
		printf("Polling sample [%i]\n", i);
		float* samples = IMU_pull_samples();
		// printf("Pulled samples: ");
		// printf("%1.4f %1.4f %1.4f\t\t%1.4f %1.4f %1.4f\t\t%1.4f %1.4f %1.4f", 
		// 	samples[0], samples[1], samples[2],
		// 	samples[3], samples[4], samples[5],
		// 	samples[6], samples[7], samples[8]);
		// printf("\n");
		arr[0][i] = samples[0];// * 100000;
		arr[1][i] = samples[1];// * 100000;
		arr[2][i] = samples[2];// * 100000;
		arr[3][i] = samples[3];// * 100000;
		arr[4][i] = samples[4];// * 100000;
		arr[5][i] = samples[5];// * 100000;
		arr[6][i] = samples[6];// * 100000;
		arr[7][i] = samples[7];// * 100000;
		arr[8][i] = samples[8];// * 100000;
		usleep(500000);
	}

	float arr_ave[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	for(int i=0; i<9; i++){
		for(int j=0; j<loop_count; j++)
			arr_ave[i] += arr[i][j];
		arr_ave[i] /= loop_count;
	}

	float arr_stddev[9];
	for(int i=0; i<9; i++)
		for(int j=0; j<loop_count; j++)
			arr_stddev[i] = (arr[i][j] - arr_ave[i])*(arr[i][j] - arr_ave[i]);

	for(int i=0; i<9; i++)
		arr_stddev[i] = sqrt(arr_stddev[i] / (loop_count - 1));

	for(int i=0; i<9; i++)
		printf("[%i] Ave: %2.6f Std_Dev: %2.6f\n", i, arr_ave[i], arr_stddev[i]);

	return 0;
}
