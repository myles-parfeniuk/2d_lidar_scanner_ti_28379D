//in-house includes
#include "tf_mini_s_lidar.h"

bool lidar_request_distance(const char sensor_addr)
{
    if(i2c_master_tx(sensor_addr, lidar_cmd_distance, LIDAR_CMD_DIST_SZ))
        return true;
    else
        return false;

}

bool lidar_read_distance(const char sensor_addr, uint16_t *dist)
{
    char rx_distance[LIDAR_DAT_DIST_SZ];

    if(i2c_master_rx(sensor_addr, rx_distance, LIDAR_DAT_DIST_SZ))
    {
        *dist = (uint16_t)(rx_distance[2]|(rx_distance[3] << 8));
        return true;
    }
    else
        return false;
}

bool lidar_init(const char sensor_addr)
{
    //set lidar sensor interal sample rate to max
    if(i2c_master_tx(sensor_addr, lidar_cmd_frame_rate, LIDAR_CMD_FRAME_RATE_SZ))
    {
        Task_sleep(100U);
        return true;
    }
    else
        return false;
}

