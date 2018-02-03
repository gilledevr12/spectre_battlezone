/*
 *  * SPI testing utility (using spidev driver)
 *   *
 *    * Copyright (c) 2007  MontaVista Software, Inc.
 *     * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *      *
 *       * This program is free software; you can redistribute it and/or modify
 *        * it under the terms of the GNU General Public License as published by
 *         * the Free Software Foundation; either version 2 of the License.
 *          *
 *           * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 *            */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static void pabort(const char *s)
{
	perror(s);
	abort();
}

static const char *device = "/dev/spidev0.0";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delay;

static void transfer(int fd, uint8_t tx[], uint8_t *rxa)
{
	int ret;
	uint8_t rx[10];
	for (int i = 0; i < 10; i++){
		rx[i] = rxa[i];
	}
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = ARRAY_SIZE(rx),
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
	pabort("can't send spi message");
	for (ret = 0; ret < ARRAY_SIZE(rx); ret++) {
		if (!(ret % 6)) puts("");
			printf("%.2X ", rx[ret]);
	}
	puts("");
}

int main(int argc, char *argv[])
{
		int ret = 0;
		int fd;

		//parse_opts(argc, argv);

		fd = open(device, O_RDWR);
		if (fd < 0)
			pabort("can't open device");

							/*
							 * 	 * spi mode
							 * 	 	 */
		ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
		if (ret == -1)
			pabort("can't set spi mode");
		ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
		if (ret == -1)
			pabort("can't get spi mode");

		/*
		 * 	 * bits per word
		* 	 	 */
		ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
		if (ret == -1)
			pabort("can't set bits per word");

		ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
		if (ret == -1)
			pabort("can't get bits per word");

		/*
		 * 	 * max speed hz
		 * 	 	 */
		ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
		if (ret == -1)
			pabort("can't set max speed hz");

		ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
		if (ret == -1)
			pabort("can't get max speed hz");

		printf("spi mode: %d\n", mode);
		printf("bits per word: %d\n", bits);
		printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
		uint8_t tx1[10] = {0xA1, 0x01, 0x23, 0x45, 0x67, 0x89, 0x00, 0x12, 0x34, 0x56  };
		uint8_t rx1[10] = {0, 0, 0 };
		transfer(fd, tx1, rx1);
		uint8_t tx2[3] = {0x21, };
		uint8_t rx2[10] = {0, 0, 0, };
		transfer(fd, tx2, rx2);
	close(fd);
	return ret;
}
