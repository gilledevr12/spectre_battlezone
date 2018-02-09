//
// Created by gilledevr12 on 2/5/18.
//

#ifndef DWM1000_MY_DECA_SPI_H
#define DWM1000_MY_DECA_SPI_H

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "deca_types.h"
#include <assert.h>


#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static const char *device = "/dev/spidev0.0";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint32_t speed_high = 2000000;
static uint16_t delay = 0;
static int max_speed = 0;

static void pabort(const char *s);
int myreadfromspi(uint16 headerLength, const uint8 *headerBuffer, uint32 readlength, uint8 *readBuffer);
int mywritetospi(uint16 headerLength, const uint8 *headerBuffer, uint32 bodylength, const uint8 *bodyBuffer);
static void transfer(int fd, uint8_t *tx, uint8_t *rx, int length);
int setupSpi();
void setSpeed(int mode);

#endif //DWM1000_MY_DECA_SPI_H
