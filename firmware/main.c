
/***********************************************************************************
 * main.c
 *
 * Firmware to interface TI tm320F828379D controller with Benewake TF-MINI-S
 * LiDAR sensor.
 * Continuously reads sensor data at a rate of ~800Hz and outputs to SCIA module
 * (USB comport) at a rate of ~3MBaud.
 *
 *
 * Author:    Myles Parfeniuk
 * Date:      10/09/2023
 * Modified:  10/09/2023
 *********************************************************************************/


//standard c libraries
#include <stdio.h>


//TI c2000Ware libraries
#include "driverlib.h"
#include "device.h"

//in-house libraries
#include "28379D_uart.h"
#include "28379D_i2c.h"
#include "28379D_timer.h"
#include "tf_mini_s_lidar.h"


//isr routine for taking distance samples
__interrupt void lidar_sampler_ISR(void);

timer_t lidar_sample_timer; //timer responsible for triggering lidar_sampler_ISR's respective interrupt
char disp_buff[40]; //display buffer used to send distance reading to PC over UART


void main(void)
{

    Device_init(); //init system clock and peripherals
    Device_initGPIO(); //unlock gpio config registers, enable all respective pullups
    Interrupt_initModule(); //initialize port interrupt enable registers (PIE)
    Interrupt_initVectorTable(); //initialize PIE vector table with pointers to ISRs
    Interrupt_enableGlobal(); //enable global interrupts

    uart_init(3100000U); //initialize uart with ~3MBaud transfer rate
    i2c_init(); //initialize I2C

    //initialize lidar sampling timer
    lidar_sample_timer.cb_ptr = &lidar_sampler_ISR;
    lidar_sample_timer.freq = 1000;
    lidar_sample_timer.module = CPUTIMER0_BASE;
    timer_init(&lidar_sample_timer);


    //configure IO_6 to be used as LED drive
    GPIO_setPadConfig(6, GPIO_PIN_TYPE_PULLUP); //enable pullup
    GPIO_writePin(6, 1); //set initial value to high
    GPIO_setPinConfig(GPIO_6_GPIO6); //set pin config as GPIO pin, no peripheral to be muxed
    GPIO_setDirectionMode(6, GPIO_DIR_MODE_OUT); //set as output

    //set frame rate of lidar sensor
    i2c_master_tx(ADDR_LIDAR_1, lidar_frame_rate_cmd, LIDAR_CMD_FRAME_RATE_SZ);
    DEVICE_DELAY_US(100000); //delay 100ms according to TF-MINI-S data sheet
    i2c_master_tx(ADDR_LIDAR_2, lidar_frame_rate_cmd, LIDAR_CMD_FRAME_RATE_SZ);
    DEVICE_DELAY_US(100000); //delay 100ms according to TF-MINI-S data sheet

    while(1)
    {
        //toggle LED every 100ms
        GPIO_togglePin(6);
        DEVICE_DELAY_US(100000);

    }

}


//lidar sampling isr
//retrieves data from lidar sensor and writes to USB serial port
__interrupt void lidar_sampler_ISR(void)
{
    //when true send request to lidar sensor for distance data (master tx), when false receive distance sample (master rx)
    static uint8_t state = STATE_0_REQ_DIST_LIDAR_1;


    CPUTimer_disableInterrupt(lidar_sample_timer.module); //enable interrupt for respective timer module

    switch(state)
    {
        case STATE_0_REQ_DIST_LIDAR_1:
            i2c_master_tx(ADDR_LIDAR_1, lidar_cmd_distance, LIDAR_CMD_DIST_SZ); //(1/400KHz * (8*(LIDAR_CMD_DIST_SZ + address_byte|W)) == 120us
            lidar_sample_timer.freq = 20000; ///50us wait before state 1
            state++;

            break;

        case STATE_1_REQ_DIST_LIDAR_2:

            i2c_master_tx(ADDR_LIDAR_2, lidar_cmd_distance, LIDAR_CMD_DIST_SZ); //120us
            lidar_sample_timer.freq = 1980; //505us wait before state 2 (1000us-2*120us-50us= 710us)
            state++;

            break;

        case STATE_2_RD_DIST_LIDAR_1:

            i2c_master_rx(ADDR_LIDAR_1, rx_distance_1, LIDAR_DAT_DIST_SZ); //(1/400KHz * (8*(LIDAR_DAT_DIST_SZ + address_byte|R)) == 205us/
            lidar_sample_timer.freq = 25000; //40us wait before state 3
            state++;

            break;

        case STATE_3_RD_DIST_LIDAR_2:

            i2c_master_rx(ADDR_LIDAR_2, rx_distance_2, LIDAR_DAT_DIST_SZ);
            sprintf(disp_buff, "111 %03d %03d\n\r", (unsigned int)(rx_distance_1[2]|(rx_distance_1[3] << 8)), (unsigned int)(rx_distance_2[2]|(rx_distance_2[3] << 8))); //save acquired data to buffer
            uart_tx_str(disp_buff); //send out data to python client
            lidar_sample_timer.freq = 100000; //10us wait before state 3
            state = STATE_0_REQ_DIST_LIDAR_1;

            break;

    }

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1); //clear CPUTIMER0 respective interrupt flags
    timer_set_freq(&lidar_sample_timer);
    CPUTimer_enableInterrupt(lidar_sample_timer.module); //enable interrupt for respective timer module

}
