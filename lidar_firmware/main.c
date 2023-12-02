/*
 *  ======== main.c ========
 */
//C standard library includes
#include <stdio.h>
#include <stdint.h>
//TI includes
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <Headers/F2837xD_device.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>

//in-house includes
#include "peripherals/internal/28379D_i2c.h"
#include "peripherals/internal/28379D_spi.h"
#include "peripherals/internal/28379D_uart.h"
#include "peripherals/internal/gpio.h"
#include "peripherals/external/tf_mini_s_lidar.h"
#include "peripherals/external/oled/SH1122_oled.h"
#include "peripherals/external/stepper.h"

extern const Task_Handle lidar_sample_tsk; //Task for sample lidar OK
extern const Task_Handle stepper_tsk; //Task for stepper OK

void step_task()
{

    while(1)
    {
        stepper_step();

    }
}

void lidar_sample_task()
{
   uint16_t distance_1 = 0;
   uint16_t distance_2 = 0;


   //initialize LiDAR sensors
   lidar_init(ADDR_LIDAR_1);
   lidar_init(ADDR_LIDAR_2);

   oled_init();
   oled_set_orientation(true);

   for(;;)
    {
        lidar_request_distance(ADDR_LIDAR_1);
        Task_sleep(5U);
        lidar_request_distance(ADDR_LIDAR_2);
        Task_sleep(5U);
        lidar_read_distance(ADDR_LIDAR_1, &distance_1);
        Task_sleep(2U);
        lidar_read_distance(ADDR_LIDAR_2, &distance_2);
        uart_tx_str("111 %03d %03d\n\r", distance_1, distance_2);
        GpioDataRegs.GPATOGGLE.bit.GPIO6 |= 1U;

        oled_clear_buffer();
        oled_draw_frame(2, 2, OLED_WIDTH - 2, OLED_HEIGHT - 2, oled_intens_15);
        oled_draw_rectangle(2, 2, OLED_WIDTH - 2, OLED_HEIGHT - 2, oled_intens_2);

        oled_draw_progres_bar(20, 20, (OLED_HEIGHT)/10, OLED_WIDTH - 40.0, 0, 1000, 0, distance_1);
        oled_draw_progres_bar(20, 30, (OLED_HEIGHT)/10, OLED_WIDTH - 40.0, 0, 1000, 0, distance_2);

        oled_draw_string(20, (20 + OLED_HEIGHT - 40), oled_intens_15, font_f6x8m ,"D1: %03dcm D2: %03dcm", distance_1, distance_2);


        oled_send_buffer();
        Task_sleep(28U);


    }


}

/*
 *  ======== main ========
 */
int main()
{ 
    gpio_init();
    uart_init(1000000U);
    i2c_init();
    spi_init();
    stepper_init();



    BIOS_start(); //does not return

    return(0);
}
