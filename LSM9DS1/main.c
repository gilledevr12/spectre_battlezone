#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <time.h>
#include "lsm9ds1.h"

int main(){
	init_IMU();
	calibrate_IMU();

	while(1){
		int* samples = IMU_pull_samples();
		printf("Pulled samples: ");
		for(int i=0; i<9; i++){
			printf("%i ", samples[i]);
		}
		printf("\n");
		usleep(500000);
	}

	return 0;
}
