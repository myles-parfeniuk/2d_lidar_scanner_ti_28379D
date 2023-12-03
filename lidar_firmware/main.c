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
#include <ti/sysbios/knl/Swi.h>
#include <Headers/F2837xD_device.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Mailbox.h>

//in-house includes
#include "peripherals/internal/28379D_i2c.h"
#include "peripherals/internal/28379D_spi.h"
#include "peripherals/internal/28379D_uart.h"
#include "peripherals/internal/gpio.h"
#include "peripherals/external/tf_mini_s_lidar.h"
#include "peripherals/external/oled/SH1122_oled.h"
#include "peripherals/external/stepper.h"

extern const Task_Handle lidar_sample_tsk_hdl; //Task for sample lidar OK
extern const Task_Handle step_tsk_hdl; //Task for stepper OK
extern const Task_Handle oled_display_tsk_hdl;
extern const Task_Handle pc_data_tx_tsk_hdl;

Mailbox_Handle distance_to_oled_mailbox;
Mailbox_Handle distance_to_pc_mailbox;

void step_task()
{
    while(1)
    {
        stepper_step();
    }
}

void lidar_sample_task()
{
   uint16_t distance_buffer[2];

   //initialize LiDAR sensors
   lidar_init(ADDR_LIDAR_1);
   lidar_init(ADDR_LIDAR_2);

   while(1)
    {
        lidar_request_distance(ADDR_LIDAR_1);
        Task_sleep(5U);
        lidar_request_distance(ADDR_LIDAR_2);
        Task_sleep(5U);
        lidar_read_distance(ADDR_LIDAR_1, distance_buffer);
        Task_sleep(2U);
        lidar_read_distance(ADDR_LIDAR_2, (distance_buffer + 1));

        Mailbox_post(distance_to_oled_mailbox, distance_buffer, BIOS_NO_WAIT);
        Mailbox_post(distance_to_pc_mailbox, distance_buffer, BIOS_WAIT_FOREVER);
    }


}

void oled_display_task()
{
    uint16_t distance_buffer[2];
    oled_init();
    oled_set_orientation(true);

    while(1)
    {

        Mailbox_pend(distance_to_oled_mailbox, distance_buffer, BIOS_NO_WAIT);

        oled_clear_buffer();
        oled_draw_frame(2, 2, OLED_WIDTH - 2, OLED_HEIGHT - 2, oled_intens_15);
        oled_draw_rectangle(2, 2, OLED_WIDTH - 2, OLED_HEIGHT - 2, oled_intens_2);

        oled_draw_progres_bar(20, 20, (OLED_HEIGHT)/10, OLED_WIDTH - 40.0, 0, 1000, 0, distance_buffer[0]);
        oled_draw_progres_bar(20, 30, (OLED_HEIGHT)/10, OLED_WIDTH - 40.0, 0, 1000, 0, distance_buffer[1]);

        oled_draw_string(20, (20 + OLED_HEIGHT - 40), oled_intens_15, font_f6x8m ,"D1: %03dcm D2: %03dcm", distance_buffer[0], distance_buffer[1]);

        oled_send_buffer();

        Task_sleep(28U);
    }
}

void pc_data_tx_task()
{
    static uint16_t distance_buffer[2];

    while(1)
    {
        Mailbox_pend(distance_to_pc_mailbox, distance_buffer, BIOS_WAIT_FOREVER);
        GpioDataRegs.GPATOGGLE.bit.GPIO6 |= 1U;
        uart_tx_str("111 %03d %03d\n\r", distance_buffer[0], distance_buffer[1]);
    }
}

/*
 *  ======== main ========
 */
int main()
{ 
    gpio_init();
    uart_init(1000000UL);
    i2c_init();
    spi_init(4000000UL);
    stepper_init();

    //create mailboxes for communication between TSKs
    distance_to_oled_mailbox = Mailbox_create(2 * sizeof(uint16_t), 1, NULL, NULL);
    distance_to_pc_mailbox = Mailbox_create(2 * sizeof(uint16_t), 1, NULL, NULL);

    BIOS_start(); //does not return

    return(0);
}
