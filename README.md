# 2d Lidar Scanner with TI F2837x Launchpad

## About
Two dimensional lidar scanner project utilizing the TI F2837x launchpad development board with a tms320f28379D controller.

## To do

### General Design Issues
- Determine whether DC motor with encoder or stepper motor will be used
- Determine which language should be used for software client (pytho or C++)

### Firmware
- divide functionality between different thread types in meaningful fashion (HWI, SWI, TSK, IDLE)
- implement firmware for position control of stepper motor or DC motor with encoder
- implement menu system for OLED display and encoder or buttons for user input

### Software Client
- modify existing client(s) for displaying angular position as well as distance (currently just draws line representing distance)

### Hardware/Case
- Design support such that LiDAR sensor(s) can be mounted to motor shaft
- Design an enclosure or case 

