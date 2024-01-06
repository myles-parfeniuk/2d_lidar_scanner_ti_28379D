//in-house includes
#include "tf_mini_s_lidar.h"

//sends a request to the lidar sensor to take a measurement
bool lidar_request_distance(const Uint8 sensor_addr)
{
    if(i2c_master_tx(sensor_addr, lidar_cmd_distance, LIDAR_CMD_DIST_SZ))
        return true;
    else
        return false;

}

//reads back a measurement from the lidar sensor, request distance should be called first for a fresh sample
bool lidar_read_distance(const Uint8 sensor_addr, uint16_t *dist)
{
    Uint8 rx_distance[LIDAR_DAT_DIST_SZ];

    if(i2c_master_rx(sensor_addr, rx_distance, LIDAR_DAT_DIST_SZ))
    {
        *dist = (uint16_t)(rx_distance[2]|(rx_distance[3] << 8)); //combine MSB and LSB of received distance
        return true;
    }
    else
        return false;
}

//sets the LiDAR sensors to the desired frame rate to initialize them
bool lidar_init(const Uint8 sensor_addr)
{
    //set lidar sensor interal sample rate to max
    if(i2c_master_tx(sensor_addr, lidar_cmd_frame_rate, LIDAR_CMD_FRAME_RATE_SZ))
    {
        Task_sleep(100U); //wait for frame rate to change (see TF mini data sheet, 100ms wait)
        return true;
    }
    else
        return false;
}

