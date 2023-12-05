/*
 *  ======== main.c ========
 */
//C standard library includes
#include <stdio.h>
#include <stdint.h>
#include <string.h>
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
extern const Semaphore_Handle lidar_sample_delay_sem; //Semaphore used for delays between lidar sampling events


extern const Semaphore_Handle step_to_sample_sem; //semaphore used for synchronized stepper and sample tasks
extern const Semaphore_Handle sample_to_step_sem; //semaphore used for synchronized stepper and sample tasks

Mailbox_Handle distance_to_oled_mailbox;
Mailbox_Handle distance_to_pc_mailbox;
Mailbox_Handle angle_to_pc_mailbox;

void step_task()
{
    uint16_t angle;
    Task_sleep(100U); //wait ~100ms on boot for lidar sensors to stabilize
    stepper_zero(); // zero the stepper
    Semaphore_post(step_to_sample_sem); //post lidar sample task so it can take initial measurement for EMA filter
    Semaphore_pend(sample_to_step_sem, BIOS_WAIT_FOREVER); //wait to be posted by lidar sample task
    while(1)
    {
        angle = stepper_step(10); //take 10 steps
        Mailbox_post(angle_to_pc_mailbox, &angle, 10U); //send angle to PC tx task, timeout after 10ms
        //post lidar sample task
        Semaphore_post(step_to_sample_sem);
        //wait to be posted by lidar sample task
        Semaphore_pend(sample_to_step_sem, BIOS_WAIT_FOREVER);
    }
}

void lidar_sample_task()
{
   uint16_t distance_buffer[2] = {LIDAR_MAX_MEASUREMENT_RANGE, LIDAR_MAX_MEASUREMENT_RANGE};
   uint16_t ema_window_distance_1[3] = {0, 0, 0};
   uint16_t ema_window_distance_2[3] = {0, 0, 0};
   uint16_t filtered_distance[2] = {LIDAR_MAX_MEASUREMENT_RANGE, LIDAR_MAX_MEASUREMENT_RANGE}; //EMA (exponential moving average) filtered data
   uint16_t filter_alpha = 0.3;
   uint16_t i = 0;
   uint16_t filtered_average_1 = 0;
   uint16_t filtered_average_2 = 0;
   //initialize LiDAR sensors
   lidar_init(ADDR_LIDAR_1);
   lidar_init(ADDR_LIDAR_2);

   //take initial measurement for EMA filter start values
   Semaphore_pend(step_to_sample_sem, BIOS_WAIT_FOREVER); //wait to be posted by step_task
   //take samples until data is within max measurement range (sensor returns above if measurement error occurred)
   while((distance_buffer[0] >= LIDAR_MAX_MEASUREMENT_RANGE) || (distance_buffer[1] >= LIDAR_MAX_MEASUREMENT_RANGE))
   {
       lidar_request_distance(ADDR_LIDAR_1); //request distance from sensor 1
       delay_task_ticks(30U, lidar_sample_delay_sem); //delay 300us
       lidar_request_distance(ADDR_LIDAR_2); //request distance from sensor 2
       delay_task_ticks(100U, lidar_sample_delay_sem); //delay 1ms (tf-mini data sheet states 1ms between each read)
       lidar_read_distance(ADDR_LIDAR_1, distance_buffer); //read distance from sensor 1
       delay_task_ticks(30U, lidar_sample_delay_sem); //delay 300us
       lidar_read_distance(ADDR_LIDAR_2, (distance_buffer + 1)); //read distance from sensor 2
   }

   //save initial distance value to filter buffer
   filtered_distance[0] = distance_buffer[0];
   filtered_distance[1] = distance_buffer[1];
   memset(ema_window_distance_1, distance_buffer[0], sizeof(ema_window_distance_1));
   memset(ema_window_distance_2, distance_buffer[1], sizeof(ema_window_distance_2));

   Semaphore_post(sample_to_step_sem);

   while(1)
    {
        Semaphore_pend(step_to_sample_sem, BIOS_WAIT_FOREVER); //wait to be posted by step_task

        distance_buffer[0] = LIDAR_MAX_MEASUREMENT_RANGE;
        distance_buffer[1] = LIDAR_MAX_MEASUREMENT_RANGE;


        lidar_request_distance(ADDR_LIDAR_1); //request distance from sensor 1
        delay_task_ticks(30U, lidar_sample_delay_sem); //delay 300us
        lidar_request_distance(ADDR_LIDAR_2); //request distance from sensor 2
        delay_task_ticks(100U, lidar_sample_delay_sem); //delay 1ms (tf-mini data sheet states 1ms between each read)
        lidar_read_distance(ADDR_LIDAR_1, distance_buffer); //read distance from sensor 1
        delay_task_ticks(30U, lidar_sample_delay_sem); //delay 300us
        lidar_read_distance(ADDR_LIDAR_2, (distance_buffer + 1)); //read distance from sensor 2

        if(distance_buffer[0] > LIDAR_MAX_MEASUREMENT_RANGE)
            distance_buffer[0] = filtered_distance[0];

        if(distance_buffer[1] > LIDAR_MAX_MEASUREMENT_RANGE)
            distance_buffer[1] = filtered_distance[1];

        filtered_distance[0] = (uint16_t)(filter_alpha * (float)filtered_distance[0] + (1.0 - filter_alpha) * (float)distance_buffer[0]);
        filtered_distance[1] = (uint16_t)(filter_alpha * (float)filtered_distance[1] + (1.0 - filter_alpha) * (float)distance_buffer[1]);

        //shift elements forward for moving window
        for(i = 0; i < 2; i++)
            ema_window_distance_1[i + 1] = ema_window_distance_1[i];
        ema_window_distance_1[0] = filtered_distance[0];

        for(i = 0; i < 2; i++)
             ema_window_distance_2[i + 1] = ema_window_distance_2[i];
        ema_window_distance_2[0] = filtered_distance[1];

        filtered_average_1 = 0;
        filtered_average_2 = 0;

        //calculate averages
        for(i = 0; i < 3; i++)
            filtered_average_1 += ema_window_distance_1[i];

        filtered_average_1 = (uint16_t)((float)filtered_average_1/3.0);

        for(i = 0; i < 3; i++)
                  filtered_average_2 += ema_window_distance_2[i];

        filtered_average_2 = (uint16_t)((float)filtered_average_2/3.0);

        filtered_distance[0] = filtered_average_1;
        filtered_distance[1] = filtered_average_2;

        //send distance to OLED task via mailbox (using NO_WAIT because it is not critical that OLED task receives every sample, it just needs most recent)
        Mailbox_post(distance_to_oled_mailbox, distance_buffer, BIOS_NO_WAIT);
        //send distance to PC sending task timeout if fails after 10ms
        Mailbox_post(distance_to_pc_mailbox, filtered_distance, 10U);
        //post step_task to run
        Semaphore_post(sample_to_step_sem);
    }


}

void oled_display_task()
{
    uint16_t distance_buffer[2];
    oled_init();

    oled_clear_buffer();
    oled_draw_frame(2, 2, OLED_WIDTH - 2, OLED_HEIGHT - 2, oled_intens_15);
    oled_draw_rectangle(2, 2, OLED_WIDTH - 2, OLED_HEIGHT - 2, oled_intens_2);
    oled_draw_string(100, (20), oled_intens_15, font_f10x16f ,"ZEROING");
    oled_send_buffer();
    Semaphore_pend(zero_sem, BIOS_WAIT_FOREVER);


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
    static uint16_t angle;
    static uint16_t filtered_distance[2];

    while(1)
    {
        Mailbox_pend(angle_to_pc_mailbox, &angle, BIOS_WAIT_FOREVER);
        Mailbox_pend(distance_to_pc_mailbox, filtered_distance, BIOS_WAIT_FOREVER);
        GpioDataRegs.GPATOGGLE.bit.GPIO6 |= 1U;
        uart_tx_str(" %06d %03d %03d\n\r", angle, filtered_distance[0], filtered_distance[1]);
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
    angle_to_pc_mailbox = Mailbox_create(1 * sizeof(uint16_t), 1, NULL, NULL);

    BIOS_start(); //does not return

    return(0);
}
