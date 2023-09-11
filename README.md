# 2d Lidar Scanner with TI F2837x Launchpad

## About
Two dimensional lidar scanner project utilizing the TI F2837x launchpad development board with a tms320f28379D controller.

## To do

### General Design Issues
- Determine whether DC motor with encoder or stepper motor will be used
- Determine whether a single, or two LiDAR sensors will be used at once

### Firmware
- port any C200ware SDK function calls to custom bare-metal drivers
- implement firmware for position control of stepper motor or DC motor with encoder
- implement RTOS
- implement OLED display and buttons for scanning speed/resolution control (if time permits)

### Python Client
- modify existing client for displaying angular position as well as distance (currently just draws line representing distance)

### Hardware/Case
- Design support such that LiDAR sensor(s) can be mounted to motor shaft
- Design an enclosure or case 

