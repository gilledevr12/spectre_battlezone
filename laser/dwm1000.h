#ifndef DWM1000_H
#define DWM1000_H

#include "../dwm1000/decadriver/dwm_api/my_deca_spi.h"
#include "../dwm1000/decadriver/dwm_api/deca_device_api.h"
#include "../dwm1000/decadriver/dwm_api/deca_regs.h"

static dwt_config_t config = {
        2,               /* Channel number. */
        DWT_PRF_64M,     /* Pulse repetition frequency. */
        DWT_PLEN_1024,   /* Preamble length. Used in TX only. */
        DWT_PAC32,       /* Preamble acquisition chunk size. Used in RX only. */
        9,               /* TX preamble code. Used in TX only. */
        9,               /* RX preamble code. Used in RX only. */
        1,               /* 0 to use standard SFD, 1 to use non-standard SFD. */
        DWT_BR_110K,     /* Data rate. */
        DWT_PHRMODE_STD, /* PHY header mode. */
        (1025 + 64 - 32) /* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
};

static dwt_txconfig_t txconfig = {
        0xC2, //PGdly
//TX POWER
//31:24 BOOST_0.125ms_PWR
//23:16 BOOST_0.25ms_PWR-TX_SHR_PWR
//15:8 BOOST_0.5ms_PWR-TX_PHR_PWR
//7:0 DEFAULT_PWR-TX_DATA_PWR
        0x67676767 //power
};

#define TX_ANT_DLY 16436
#define RX_ANT_DLY 16436

static uint8 rx_poll_msg[3][12] = {
        {0x41, 0x88, 0, 0xCA, 0xDE, 'A', '1', 'T', '1', 0x21, 0, 0},
        {0x41, 0x88, 0, 0xCA, 0xDE, 'A', '2', 'T', '1', 0x21, 0, 0},
        {0x41, 0x88, 0, 0xCA, 0xDE, 'A', '3', 'T', '1', 0x21, 0, 0}
};
static uint8 tx_resp_msg[3][15] = {
        {0x41, 0x88, 0, 0xCA, 0xDE, 'T', '1', 'A', '1', 0x10, 0x02, 0, 0, 0, 0},
        {0x41, 0x88, 0, 0xCA, 0xDE, 'T', '1', 'A', '2', 0x10, 0x02, 0, 0, 0, 0},
        {0x41, 0x88, 0, 0xCA, 0xDE, 'T', '1', 'A', '3', 0x10, 0x02, 0, 0, 0, 0}
};
static uint8 rx_final_msg[3][24] = {
        {0x41, 0x88, 0, 0xCA, 0xDE, 'A', '1', 'T', '1', 0x23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0x41, 0x88, 0, 0xCA, 0xDE, 'A', '2', 'T', '1', 0x23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0x41, 0x88, 0, 0xCA, 0xDE, 'A', '3', 'T', '1', 0x23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

/* Length of the common part of the message (up to and including the function code, see NOTE 2 below). */
#define ALL_MSG_COMMON_LEN 9
/* Index to access some of the fields in the frames involved in the process. */
#define ALL_MSG_SN_IDX 2
#define FINAL_MSG_POLL_TX_TS_IDX 10
#define FINAL_MSG_RESP_RX_TS_IDX 14
#define FINAL_MSG_FINAL_TX_TS_IDX 18
#define FINAL_MSG_TS_LEN 4
/* Frame sequence number, incremented after each transmission. */
static uint8 frame_seq_nb = 0;

/* Buffer to store received messages.
 * Its size is adjusted to longest frame that this example code is supposed to handle. */
#define RX_BUF_LEN 24
static uint8 rx_buffer[RX_BUF_LEN];

/* Hold copy of status register state here for reference so that it can be examined at a debug breakpoint. */
static uint32 status_reg = 0;

/* UWB microsecond (uus) to device time unit (dtu, around 15.65 ps) conversion factor.
 * 1 uus = 512 / 499.2 �s and 1 �s = 499.2 * 128 dtu. */
#define UUS_TO_DWT_TIME 65536

/* Delay between frames, in UWB microseconds. See NOTE 4 below. */
/* This is the delay from Frame RX timestamp to TX reply timestamp used for calculating/setting the DW1000's delayed TX function. This includes the
 * frame length of approximately 2.46 ms with above configuration. */
#define POLL_RX_TO_RESP_TX_DLY_UUS 9000//2600
/* This is the delay from the end of the frame transmission to the enable of the receiver, as programmed for the DW1000's wait for response feature. */
#define RESP_TX_TO_FINAL_RX_DLY_UUS 500
/* Receive final timeout. See NOTE 5 below. */
#define FINAL_RX_TIMEOUT_UUS 60000//3300
/* Preamble timeout, in multiple of PAC size. See NOTE 6 below. */
//#define PRE_TIMEOUT 8

/* Timestamps of frames transmission/reception.
 * As they are 40-bit wide, we need to define a 64-bit int type to handle them. */
typedef signed long long int64;
typedef unsigned long long uint64;
static uint64 poll_rx_ts[3];
static uint64 resp_tx_ts[3];
static uint64 final_rx_ts[3];

/* Speed of light in air, in metres per second. */
#define SPEED_OF_LIGHT 299702547
#define HIGH 1


/* Hold copies of computed time of flight and distance here for reference so that it can be examined at a debug breakpoint. */
static double tof[3];
static double distance[3];
static int anchCnt = 0;

/* String used to display measured distance on LCD screen (16 characters maximum). */
char dist_str_1[33] = {0};
char dist_str_2[33] = {0};
char dist_str_3[33] = {0};
char dist_str[100] = {0};
static bool quitting = false;

static int tagCnt[3][3] = {
        {1, 2, 3},
        {2, 3, 1},
        {3, 1, 2}
};

static void final_msg_get_ts(const uint8 *ts_field, uint32 *ts);
static uint64 get_rx_timestamp_u64(void);
static uint64 get_tx_timestamp_u64(void);
int init_dwm();
void runRanging(char *token, int num, char* play);

#endif //DWM1000_H
