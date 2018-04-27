//some definitions about the dwm1000
#include "dwm1000.h"
#include <string.h>
#include <time.h>
#include "mqtt.h"

/* String used to display measured distance on LCD screen (16 characters maximum). */
char dist_str_1[33] = {0};
char dist_str_2[33] = {0};
char dist_str_3[33] = {0};
char dist_str[100] = {0};
int anchCnt = 1;
bool quitting = false;
bool success = true;
bool nothingHappened = false;
extern unsigned char POLL_SAMPLES;
extern unsigned char MUTEX;

float UWB_Curr_Distance[3];

uint8 rx_poll_msg[3][12] = {
        {0x41, 0x88, 0, 0xCA, 0xDE, 'A', '1', 'T', '1', 0x21, 0, 0},
        {0x41, 0x88, 0, 0xCA, 0xDE, 'A', '2', 'T', '1', 0x21, 0, 0},
        {0x41, 0x88, 0, 0xCA, 0xDE, 'A', '3', 'T', '1', 0x21, 0, 0}
};
uint8 tx_resp_msg[3][15] = {
        {0x41, 0x88, 0, 0xCA, 0xDE, 'T', '1', 'A', '1', 0x10, 0x02, 0, 0, 0, 0},
        {0x41, 0x88, 0, 0xCA, 0xDE, 'T', '1', 'A', '2', 0x10, 0x02, 0, 0, 0, 0},
        {0x41, 0x88, 0, 0xCA, 0xDE, 'T', '1', 'A', '3', 0x10, 0x02, 0, 0, 0, 0}
};
uint8 rx_final_msg[3][24] = {
        {0x41, 0x88, 0, 0xCA, 0xDE, 'A', '1', 'T', '1', 0x23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0x41, 0x88, 0, 0xCA, 0xDE, 'A', '2', 'T', '1', 0x23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0x41, 0x88, 0, 0xCA, 0xDE, 'A', '3', 'T', '1', 0x23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

bool runRanging(char *token, int num, char* play, char* poll){

    success = false;

    double LIMIT;
    if (memcmp(play, "locate", 6) == 0) {
        LIMIT = 3;
    } else {
        LIMIT = .2;
    }

    /* Clear reception timeout to start next ranging process. */
    dwt_setrxtimeout(0);

    /* Activate reception immediately. */
    dwt_rxenable(DWT_START_RX_IMMEDIATE);
    clock_t t;
    t = clock();
    double time_taken = ((double)(clock() - t))/CLOCKS_PER_SEC;

    /* Poll for reception of a frame or error/timeout. See NOTE 8 below. */
//    while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) &
//             (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR)) && time_taken < LIMIT) {
//        time_taken = ((double)(clock() - t))/CLOCKS_PER_SEC;
//    };
    while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) &
             (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)) && time_taken < LIMIT) {
        time_taken = ((double)(clock() - t))/CLOCKS_PER_SEC;
    };
    if (time_taken > LIMIT){
        printf("restarting\n");
        quitting = true;
    }

    if (status_reg & SYS_STATUS_RXFCG) {
        uint32 frame_len;
        nothingHappened = false;

        /* Clear good RX frame event in the DW1000 status register. */
        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);

        /* A frame has been received, read it into the local buffer. */
        frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023;
        if (frame_len <= RX_BUFFER_LEN) {
            dwt_readrxdata(rx_buffer, frame_len, 0);
        }

        int ret = -1;

        /* Check that the frame is a poll sent by "DS TWR initiator" example.
         * As the sequence number field of the frame is not relevant, it is cleared to simplify the validation of the frame. */
        rx_buffer[ALL_MSG_SN_IDX] = 0;
        if (memcmp(rx_buffer, rx_poll_msg[num], ALL_MSG_COMMON_LEN) == 0) {
            uint32 resp_tx_time;

            /* Retrieve poll reception timestamp. */
            poll_rx_ts[num] = get_rx_timestamp_u64();

            /* Set send time for response. See NOTE 9 below. */
            resp_tx_time = (poll_rx_ts[num] + (POLL_RX_TO_RESP_TX_DLY_UUS * UUS_TO_DWT_TIME)) >> 8;
            dwt_setdelayedtrxtime(resp_tx_time);

            /* Set expected delay and timeout for final message reception. See NOTE 4 and 5 below. */
            dwt_setrxaftertxdelay(RESP_TX_TO_FINAL_RX_DLY_UUS);
//            dwt_setrxtimeout(FINAL_RX_TIMEOUT_UUS);

            /* Write and send the response message. See NOTE 10 below.*/
            tx_resp_msg[num][ALL_MSG_SN_IDX] = frame_seq_nb;

            dwt_writetxdata(sizeof(tx_resp_msg[num]), tx_resp_msg[num], 0); /* Zero offset in TX buffer. */
            dwt_writetxfctrl(sizeof(tx_resp_msg[num]), 0, 1); /* Zero offset in TX buffer, ranging. */
            ret = dwt_starttx(DWT_START_TX_DELAYED | DWT_RESPONSE_EXPECTED);

            /* If dwt_starttx() returns an error, abandon this ranging exchange and proceed to the next one. See NOTE 11 below. */
            if (ret == DWT_ERROR) {
                printf("error\n");
                return false;
            } else {

                t = clock();
                time_taken = ((double) (clock() - t)) / CLOCKS_PER_SEC;

                /* Poll for reception of expected "final" frame or error/timeout. See NOTE 8 below. */
//                while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) &
//                         (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR)) &&
//                       time_taken < LIMIT - .05) {
//                    time_taken = ((double) (clock() - t)) / CLOCKS_PER_SEC;
//                };
                while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) &
                         (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)) &&
                       time_taken < LIMIT - .05) {
                    time_taken = ((double) (clock() - t)) / CLOCKS_PER_SEC;
                };

                /* Increment frame sequence number after transmission of the response message (modulo 256). */
                frame_seq_nb++;

                if (status_reg & SYS_STATUS_RXFCG) {
                    /* Clear good RX frame event and TX frame sent in the DW1000 status register. */
                    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG | SYS_STATUS_TXFRS);

                    //                printf("recieved\n");
                    /* A frame has been received, read it into the local buffer. */
                    frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFLEN_MASK;
                    if (frame_len <= RX_BUF_LEN) {
                        dwt_readrxdata(rx_buffer, frame_len, 0);
                    }

                    ret = -1;

                    /* Check that the frame is a final message sent by "DS TWR initiator" example.
                     * As the sequence number field of the frame is not used in this example, it can be zeroed to ease the validation of the frame. */
                    rx_buffer[ALL_MSG_SN_IDX] = 0;
                    if (memcmp(rx_buffer, rx_final_msg[num], ALL_MSG_COMMON_LEN) == 0) {
                        success = true;
                        uint32 poll_tx_ts, resp_rx_ts, final_tx_ts;
                        uint32 poll_rx_ts_32, resp_tx_ts_32, final_rx_ts_32;
                        double Ra, Rb, Da, Db;
                        int64 tof_dtu;

                        /* Retrieve response transmission and final reception timestamps. */
                        resp_tx_ts[num] = get_tx_timestamp_u64();
                        final_rx_ts[num] = get_rx_timestamp_u64();

                        /* Get timestamps embedded in the final message. */
                        final_msg_get_ts(&rx_buffer[FINAL_MSG_POLL_TX_TS_IDX], &poll_tx_ts);
                        final_msg_get_ts(&rx_buffer[FINAL_MSG_RESP_RX_TS_IDX], &resp_rx_ts);
                        final_msg_get_ts(&rx_buffer[FINAL_MSG_FINAL_TX_TS_IDX], &final_tx_ts);

                        /* Compute time of flight. 32-bit subtractions give correct answers even if clock has wrapped. See NOTE 12 below. */
                        poll_rx_ts_32 = (uint32) poll_rx_ts[num];
                        resp_tx_ts_32 = (uint32) resp_tx_ts[num];
                        final_rx_ts_32 = (uint32) final_rx_ts[num];
                        Ra = (double) (resp_rx_ts - poll_tx_ts);
                        Rb = (double) (final_rx_ts_32 - resp_tx_ts_32);
                        Da = (double) (final_tx_ts - resp_rx_ts);
                        Db = (double) (resp_tx_ts_32 - poll_rx_ts_32);
                        tof_dtu = (int64) ((Ra * Rb - Da * Db) / (Ra + Rb + Da + Db));

                        tof[num] = tof_dtu * DWT_TIME_UNITS;
                        distance[num] = tof[num] * SPEED_OF_LIGHT;

                        if (strcmp(token, "Anchor1") == 0) {
                            UWB_Curr_Distance[0] = distance[num];
                        } else if (strcmp(token, "Anchor2") == 0) {
                            UWB_Curr_Distance[1] = distance[num];
                        } else if (strcmp(token, "Anchor3") == 0) {
                            UWB_Curr_Distance[2] = distance[num];
                        }
                        if (memcmp(poll, "poll", 4) == 0) {
                            while (MUTEX);
                            MUTEX = 1;
                            POLL_SAMPLES = 1;
                            MUTEX = 0;
                        }
                    } else {
                        //second fail
                        dwt_setrxtimeout(0);

                        return false;
                    }
                } else {
                    /* Clear RX error/timeout events in the DW1000 status register. */
                    dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);

                    /* Reset RX to properly reinitialise LDE operation. */
                    dwt_rxreset();

                    return false;
                }
            }
        } else {
            //first fail
            dwt_setrxtimeout(0);

            return false;
        }
    } else {
        /* Clear RX error/timeout events in the DW1000 status register. */
        dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);

        /* Reset RX to properly reinitialise LDE operation. */
        dwt_rxreset();

        return false;
    }

    if (memcmp(play, "play", 4) == 0) {
        //try tag responsible ranging
        char buf[30];
        int tag = rx_final_msg[num][8] - '0';
        char mode[8];
        char will_poll[8];
        if (success) {
            anchCnt++;
            strcpy(mode, "normal");
        } else {
            strcpy(mode, "restart");
        }
        if (anchCnt == 4) {
            anchCnt = 1;
            tag++;
            if (tag == 4) tag = 1;
        }
        if (anchCnt == 3) {
            strcpy(will_poll, "poll");
        } else {
            strcpy(will_poll, "idle");
        }
        sprintf(buf, "Anchor%d Tag%d %s %s %s", anchCnt, tag, "play", will_poll, mode);
        if (mosquitto_publish(mosq_pub, NULL, MQTT_TOPIC, strlen(buf), buf, 0, false)) {
            fprintf(stderr, "Could not publish to broker. Quitting\n");
            exit(-3);
        }
    }

    return true;
}

int init_dwm(){
    if (dwt_initialise(DWT_LOADUCODE) == DWT_ERROR) {
        printf("INIT FAILED");
        return 1;
    }
    setSpeed(HIGH);

    /* Configure DW1000. See NOTE 7 below. */
    dwt_configure(&config);
    // dwt_configuretxrf(&txconfig);

    /* Apply default antenna delay value. See NOTE 1 below. */
    dwt_setrxantennadelay(RX_ANT_DLY);
    dwt_settxantennadelay(TX_ANT_DLY);
    return 0;
}

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn get_tx_timestamp_u64()
 *
 * @brief Get the TX time-stamp in a 64-bit variable.
 *        /!\ This function assumes that length of time-stamps is 40 bits, for both TX and RX!
 *
 * @param  none
 *
 * @return  64-bit value of the read time-stamp.
 */
static uint64 get_tx_timestamp_u64(void){
    uint8 ts_tab[5];
    uint64 ts = 0;
    int i;
    dwt_readtxtimestamp(ts_tab);
    for (i = 4; i >= 0; i--)
    {
        ts <<= 8;
        ts |= ts_tab[i];
    }
    return ts;
}

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn get_rx_timestamp_u64()
 *
 * @brief Get the RX time-stamp in a 64-bit variable.
 *        /!\ This function assumes that length of time-stamps is 40 bits, for both TX and RX!
 *
 * @param  none
 *
 * @return  64-bit value of the read time-stamp.
 */
static uint64 get_rx_timestamp_u64(void){
    uint8 ts_tab[5];
    uint64 ts = 0;
    int i;
    dwt_readrxtimestamp(ts_tab);
    for (i = 4; i >= 0; i--)
    {
        ts <<= 8;
        ts |= ts_tab[i];
    }
    return ts;
}

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn final_msg_get_ts()
 *
 * @brief Read a given timestamp value from the final message. In the timestamp fields of the final message, the least
 *        significant byte is at the lower address.
 *
 * @param  ts_field  pointer on the first byte of the timestamp field to read
 *         ts  timestamp value
 *
 * @return none
 */
static void final_msg_get_ts(const uint8 *ts_field, uint32 *ts){
    int i;
    *ts = 0;
    for (i = 0; i < FINAL_MSG_TS_LEN; i++)
    {
        *ts += ts_field[i] << (i * 8);
    }
}

/*****************************************************************************************************************************************************
 * NOTES:
 *
 * 1. The sum of the values is the TX to RX antenna delay, experimentally determined by a calibration process. Here we use a hard coded typical value
 *    but, in a real application, each device should have its own antenna delay properly calibrated to get the best possible precision when performing
 *    range measurements.
 * 2. The messages here are similar to those used in the DecaRanging ARM application (shipped with EVK1000 kit). They comply with the IEEE
 *    802.15.4 standard MAC data frame encoding and they are following the ISO/IEC:24730-62:2013 standard. The messages used are:
 *     - a poll message sent by the initiator to trigger the ranging exchange.
 *     - a response message sent by the responder allowing the initiator to go on with the process
 *     - a final message sent by the initiator to complete the exchange and provide all information needed by the responder to compute the
 *       time-of-flight (distance) estimate.
 *    The first 10 bytes of those frame are common and are composed of the following fields:
 *     - byte 0/1: frame control (0x8841 to indicate a data frame using 16-bit addressing).
 *     - byte 2: sequence number, incremented for each new frame.
 *     - byte 3/4: PAN ID (0xDECA).
 *     - byte 5/6: destination address, see NOTE 3 below.
 *     - byte 7/8: source address, see NOTE 3 below.
 *     - byte 9: function code (specific values to indicate which message it is in the ranging process).
 *    The remaining bytes are specific to each message as follows:
 *    Poll message:
 *     - no more data
 *    Response message:
 *     - byte 10: activity code (0x02 to tell the initiator to go on with the ranging exchange).
 *     - byte 11/12: activity parameter, not used for activity code 0x02.
 *    Final message:
 *     - byte 10 -> 13: poll message transmission timestamp.
 *     - byte 14 -> 17: response message reception timestamp.
 *     - byte 18 -> 21: final message transmission timestamp.
 *    All messages end with a 2-byte checksum automatically set by DW1000.
 * 3. Source and destination addresses are hard coded constants in this example to keep it simple but for a real product every device should have a
 *    unique ID. Here, 16-bit addressing is used to keep the messages as short as possible but, in an actual application, this should be done only
 *    after an exchange of specific messages used to define those short addresses for each device participating to the ranging exchange.
 * 4. Delays between frames have been chosen here to ensure proper synchronisation of transmission and reception of the frames between the initiator
 *    and the responder and to ensure a correct accuracy of the computed distance. The user is referred to DecaRanging ARM Source Code Guide for more
 *    details about the timings involved in the ranging process.
 * 5. This timeout is for complete reception of a frame, i.e. timeout duration must take into account the length of the expected frame. Here the value
 *    is arbitrary but chosen large enough to make sure that there is enough time to receive the complete final frame sent by the responder at the
 *    110k data rate used (around 3.5 ms).
 * 6. The preamble timeout allows the receiver to stop listening in situations where preamble is not starting (which might be because the responder is
 *    out of range or did not receive the message to respond to). This saves the power waste of listening for a message that is not coming. We
 *    recommend a minimum preamble timeout of 5 PACs for short range applications and a larger value (e.g. in the range of 50% to 80% of the preamble
 *    length) for more challenging longer range, NLOS or noisy environments.
 * 7. In a real application, for optimum performance within regulatory limits, it may be necessary to set TX pulse bandwidth and TX power, (using
 *    the dwt_configuretxrf API call) to per device calibrated values saved in the target system or the DW1000 OTP memory.
 * 8. We use polled mode of operation here to keep the example as simple as possible but all status events can be used to generate interrupts. Please
 *    refer to DW1000 User Manual for more details on "interrupts". It is also to be noted that STATUS register is 5 bytes long but, as the event we
 *    use are all in the first bytes of the register, we can use the simple dwt_read32bitreg() API call to access it instead of reading the whole 5
 *    bytes.
 * 9. Timestamps and delayed transmission time are both expressed in device time units so we just have to add the desired response delay to poll RX
 *    timestamp to get response transmission time. The delayed transmission time resolution is 512 device time units which means that the lower 9 bits
 *    of the obtained value must be zeroed. This also allows to encode the 40-bit value in a 32-bit words by shifting the all-zero lower 8 bits.
 * 10. dwt_writetxdata() takes the full size of the message as a parameter but only copies (size - 2) bytes as the check-sum at the end of the frame is
 *     automatically appended by the DW1000. This means that our variable could be two bytes shorter without losing any data (but the sizeof would not
 *     work anymore then as we would still have to indicate the full length of the frame to dwt_writetxdata()).
 * 11. When running this example on the EVB1000 platform with the POLL_RX_TO_RESP_TX_DLY response delay provided, the dwt_starttx() is always
 *     successful. However, in cases where the delay is too short (or something else interrupts the code flow), then the dwt_starttx() might be issued
 *     too late for the configured start time. The code below provides an example of how to handle this condition: In this case it abandons the
 *     ranging exchange and simply goes back to awaiting another poll message. If this error handling code was not here, a late dwt_starttx() would
 *     result in the code flow getting stuck waiting subsequent RX event that will will never come. The companion "initiator" example (ex_05a) should
 *     timeout from awaiting the "response" and proceed to send another poll in due course to initiate another ranging exchange.
 * 12. The high order byte of each 40-bit time-stamps is discarded here. This is acceptable as, on each device, those time-stamps are not separated by
 *     more than 2**32 device time units (which is around 67 ms) which means that the calculation of the round-trip delays can be handled by a 32-bit
 *     subtraction.
 * 13. The user is referred to DecaRanging ARM application (distributed with EVK1000 product) for additional practical example of usage, and to the
 *     DW1000 API Guide for more details on the DW1000 driver functions.
 ****************************************************************************************************************************************************/
