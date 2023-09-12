/***********************************************************************************
 * tf_mini_s_lidar.h
 *
 * A header containing information about the Benewake TF-MINI-S LiDAR sensor's
 * commands and address.
 *
 *
 *
 * Author:    Myles Parfeniuk
 * Date:      10/09/2023
 * Modified:  10/09/2023
 *********************************************************************************/
#ifndef TF_MINI_S_LIDAR_H_
#define TF_MINI_S_LIDAR_H_

#define ADDR_LIDAR_1 0x10 //7 bit i2c addrss of first TF-MINI-S lidar sensor
#define ADDR_LIDAR_2 0x20 //7 bit i2c addrss of second TF-MINI-S lidar sensor
#define LIDAR_CMD_DIST_SZ 5 //lidar i2c command distance size (bytes)
#define LIDAR_DAT_DIST_SZ 9 //lidar response distance size (bytes)
#define LIDAR_CMD_FRAME_RATE_SZ 6 //lidar i2c command set frame rate size (bytes)
#define LIDAR_FRAME_RATE_CHECKSUM ((0x5A + 0x06 + 0x03 + 0x20 + 0x03) & 0x00FF) //checksum used in lidar i2c set frame rate command
#define LIDAR_FRAME_RATE_HH 0x03 //high byte of frame rate
#define LIDAR_FRAME_RATE_LL 0xE8 //low byte of frame rate

enum lidar_measurement_states
{
    STATE_0_REQ_DIST_LIDAR_1,
    STATE_1_REQ_DIST_LIDAR_2,
    STATE_2_RD_DIST_LIDAR_1,
    STATE_3_RD_DIST_LIDAR_2,
    STATE_UNITIALIZED_LIDAR
};

const uint8_t lidar_cmd_distance[LIDAR_CMD_DIST_SZ]= {0x5A,0x05,0x00,0x01,0x60}; // get distance i2c command
const uint8_t lidar_frame_rate_cmd[LIDAR_CMD_FRAME_RATE_SZ] = {0x5A, 0x06, 0x03, LIDAR_FRAME_RATE_LL, LIDAR_FRAME_RATE_HH, (uint8_t)LIDAR_FRAME_RATE_CHECKSUM}; // set frame rate i2c command
const uint8_t rx_distance_1[LIDAR_DAT_DIST_SZ];
const uint8_t rx_distance_2[LIDAR_DAT_DIST_SZ];



#endif /* TF_MINI_S_LIDAR_H_ */
