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

//in-house includes
#include "peripherals/internal/28379D_i2c.h"
#include "peripherals/internal/28379D_spi.h"
#include "peripherals/internal/28379D_uart.h"
#include "peripherals/internal/gpio.h"
#include "peripherals/external/tf_mini_s_lidar.h"
#include "peripherals/external/oled/SH1122_oled.h"


Void lidar_sample_task(UArg a0, UArg a1)
{
   uint16_t distance_1 = 0;
   uint16_t distance_2 = 0;
   int16_t bar_width = 1;
   bool increment = true;


   //initialize LiDAR sensors
   lidar_init(ADDR_LIDAR_1);
   lidar_init(ADDR_LIDAR_2);

   oled_init();
   //oled_set_orientation(true);
   oled_draw_frame(2, 2, OLED_WIDTH - 2, 10, oled_intens_15);
   oled_draw_rectangle(2, 2, OLED_WIDTH - 2, OLED_HEIGHT - 2, oled_intens_2);
   oled_draw_rectangle(20, 20, OLED_WIDTH - 40, OLED_HEIGHT - 40, oled_intens_5);
   oled_send_buffer();

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
        oled_draw_rectangle(20, 20, bar_width, OLED_HEIGHT - 40, oled_intens_5);
        oled_draw_string(20, (20 + OLED_HEIGHT - 40), oled_intens_15, font_f6x8m ,"D1: %03dcm D2: %03dcm", distance_1, distance_2);


        oled_send_buffer();
        Task_sleep(28U);

        if(increment)
            bar_width += 18;
        else
            bar_width -= 18;

        if(bar_width >= OLED_WIDTH - 40)
        {
            increment = false;
            bar_width = OLED_WIDTH - 40;
        }

        if(bar_width <= 0)
        {
            increment = true;
            bar_width = 1;
        }
    }
}

/*
 *  ======== main ========
 */
Int main()
{ 
    Task_Handle lidar_sample_task_hdl;
    Error_Block eb;

    //initialize peripherals
    gpio_init();
    uart_init(1000000U);
    i2c_init();
    spi_init();

    //launch tasks
    Error_init(&eb);
    lidar_sample_task_hdl = Task_create(lidar_sample_task, NULL, &eb);

    if (lidar_sample_task_hdl == NULL)
    {
        System_printf("Task_create() failed!\n");
        BIOS_exit(0);
    }

    BIOS_start(); //does not return

    return(0);
}
