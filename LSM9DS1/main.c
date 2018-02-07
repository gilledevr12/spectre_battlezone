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

	read_memory();
	return 1;

	while(1){
		int* samples = IMU_pull_samples();
		printf("Pulled samples: ");
		printf("%04x %04X %04X\t\t%04x %04X %04X\t\t%04x %04X %04X", 
			samples[0], samples[1], samples[2],
			samples[3], samples[4], samples[5],
			samples[6], samples[7], samples[8]);
		printf("\n");
		sleep(1);
	}

	return 0;
}
