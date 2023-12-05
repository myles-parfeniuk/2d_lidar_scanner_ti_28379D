/***********************************************************************************
 * tf_mini_s_lidar.h
 *
 *
 *
 *
 * Author:    Myles Parfeniuk
 * Date:      10/09/2023
 * Modified:  10/09/2023
 *********************************************************************************/
#ifndef TF_MINI_S_LIDAR_H_
#define TF_MINI_S_LIDAR_H_

//in-house includes
#include "../internal/28379D_i2c.h"

#define ADDR_LIDAR_1 0x10 //7 bit i2c address of first TF-MINI-S LiDAR sensor
#define ADDR_LIDAR_2 0x20 //7 bit i2c address of second TF-MINI-S LiDAR sensor

#define LIDAR_CMD_DIST_SZ 5 //LiDAR i2c command distance size (bytes)
#define LIDAR_DAT_DIST_SZ 9 //LiDAR response distance size (bytes)

#define LIDAR_CMD_FRAME_RATE_SZ 6 //LiDAR i2c command set frame rate size (bytes)
#define LIDAR_FRAME_RATE_CHECKSUM ((0x5A + 0x06 + 0x03 + 0x20 + 0x03) & 0x00FF) //checksum used in LiDAR i2c set frame rate command
#define LIDAR_FRAME_RATE_HH 0x03 //high byte of frame rate
#define LIDAR_FRAME_RATE_LL 0xE8 //low byte of frame rate

#define LIDAR_MAX_MEASUREMENT_RANGE 13000UL

enum lidar_measurement_states
{
    STATE_0_REQ_DIST_LIDAR_1,   //request distance sensor 1
    STATE_1_REQ_DIST_LIDAR_2,   //request distance sensor 2
    STATE_2_RD_DIST_LIDAR_1,    //read distance sensor 1
    STATE_3_RD_DIST_LIDAR_2,    //read distance sensor 2
    STATE_UNITIALIZED_LIDAR     //uninitialized
};

static const Uint8 lidar_cmd_distance[LIDAR_CMD_DIST_SZ]= {0x5A,0x05,0x00,0x01,0x60}; // get distance i2c command
static const Uint8 lidar_cmd_frame_rate[LIDAR_CMD_FRAME_RATE_SZ] = {0x5A, 0x06, 0x03, LIDAR_FRAME_RATE_LL, LIDAR_FRAME_RATE_HH, (char)LIDAR_FRAME_RATE_CHECKSUM}; // set frame rate i2c command

bool lidar_init(const Uint8 sensor_addr);
bool lidar_request_distance(const Uint8 sensor_addr);
bool lidar_read_distance(const Uint8 sensor_addr, uint16_t *dist);

#endif /* TF_MINI_S_LIDAR_H_ */
