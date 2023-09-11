# 2d Lidar Scanner with TI F2837x Launchpad— Sensor Testing Branch

## About
This branch covers the implementation of basic communication between the controller and the LiDAR sensor (via I2C)
as well as the communication between the controller and PC (via UART).
It also contains a python client to be run on the PC side that draws a line using measured DATA from the sensor. 

## To do

### Firmware
- port any C200ware SDK function calls to custom bare-metal drivers

