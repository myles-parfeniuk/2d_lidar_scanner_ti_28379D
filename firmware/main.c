
//standard c libraries
#include <stdio.h>
//TI includes
#include <Headers/F2837xD_device.h>
//in-house includes
#include "gpio.h"
#include "28379D_uart.h"
#include "28379D_timer.h"
#include "28379D_i2c.h"
#include "tf_mini_s_lidar.h"

__interrupt void lidar_sampler_ISR(void);
__interrupt void blink_ISR(void);

//declare global variables:
timer_t lidar_sample_timer =
    {
    .cb_ptr = &lidar_sampler_ISR,
    .freq = 1000U,
    .module = CPU_TIM_0
    };

    timer_t blink_timer =
    {
    .cb_ptr = &blink_ISR,
    .freq = 5U,
    .module = CPU_TIM_1
    };

char disp_buff[40]; //display buffer used to send distance reading to PC over UART

//main code starts here:
void main(void)
   {



       //initialization:
       init_gpio(); //initialize gpio
       uart_init(3000000U);
       timer_init(&lidar_sample_timer); //initialize timer0
       timer_init(&blink_timer); //initialize timer1
       i2c_init();




       while(1)
       {

       }

   }



//lidar sampling isr
//retrieves data from lidar sensor and writes to USB serial port
__interrupt void lidar_sampler_ISR(void)
{
    //when true send request to lidar sensor for distance data (master tx), when false receive distance sample (master rx)
    static char state = STATE_0_REQ_DIST_LIDAR_1;

    CpuTimer0Regs.TCR.bit.TIE = 0; //disable timer 0 interrupts

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

    timer_set_freq(&lidar_sample_timer);
    //clear ACK group and re-enable timer 0 interrupts
    EALLOW;
    PieCtrlRegs.PIEACK.all = 0x0001;
    CpuTimer0Regs.TCR.bit.TIE |= 1U; //enable interrupts
    EDIS;


}


__interrupt void blink_ISR(void)
{

    CpuTimer1Regs.TCR.bit.TIE = 0; //disable timer 0 interrupts

    GpioDataRegs.GPATOGGLE.bit.GPIO6 |= 1U;

    EALLOW;
    CpuTimer1Regs.TCR.bit.TIE |= 1U; //enable interrupts
    EDIS;


}
