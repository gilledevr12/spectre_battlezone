//
// Created by gilledevr12 on 2/5/18.
//

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
#include "my_deca_spi.h"
#include <assert.h>
#include "deca_types.h"



#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static void pabort(const char *s)
{
    perror(s);
    abort();
}

int myreadfromspi(uint16 headerLength, const uint8 *headerBuffer, uint32 readlength, uint8 *readBuffer) {
    int fd = setupSpi();
    uint8_t tx[headerLength + readlength];
    uint8_t rx[headerLength + readlength];
    for (int i = 0; i < headerLength; i++){
        tx[i] = headerBuffer[i];
        rx[i] = headerBuffer[i];
    }
    for (int i = 0; i < readlength; i++){
        rx[i + headerLength] = readBuffer[i];
    }
    transfer(fd, tx, rx, headerLength + readlength);
    for (int i = 0; i < readlength; i++){
	readBuffer[i] = rx[headerLength + i];
        printf("x%.2X ", readBuffer[i]);
    }
    puts("");
    close(fd);
}

int mywritetospi(uint16 headerLength, const uint8 *headerBuffer, uint32 bodylength, const uint8 *bodyBuffer){
    int fd = setupSpi();
    uint8_t tx[headerLength + bodylength];
    for (int i = 0; i < headerLength; i++){
        tx[i] = headerBuffer[i];
    }
    for (int i = 0; i < bodylength; i++){
        tx[i + headerLength] = bodyBuffer[i];
    }
    uint8_t rx[headerLength + bodylength];
    transfer(fd, tx, rx, headerLength + bodylength);
    close(fd);
}

static void transfer(int fd, uint8_t *tx, uint8_t *rx, int length)
{
    int ret;
    struct spi_ioc_transfer tr = {
            .tx_buf = (unsigned long)tx,
            .rx_buf = (unsigned long)rx,
            .len = length,
            .delay_usecs = delay,
            .speed_hz = speed,
            .bits_per_word = bits,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        pabort("can't send spi message");
    //for (ret = 0; ret < length; ret++) {
    //    if (!(ret % 6)) puts("");
    //    printf("%.2X ", rx[ret]);
    //}
    puts("");
}

int setupSpi() {
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

    //printf("spi mode: %d\n", mode);
    //printf("bits per word: %d\n", bits);
    //printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

    return fd;
}

