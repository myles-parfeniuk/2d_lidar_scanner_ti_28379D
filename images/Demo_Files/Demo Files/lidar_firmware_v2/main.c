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
#include "peripherals/external/encoder.h"

extern const Task_Handle lidar_sample_tsk_hdl; //Task for sample lidar OK
extern const Task_Handle step_tsk_hdl; //Task for stepper OK
extern const Task_Handle oled_display_tsk_hdl; //Task for OLED OK
extern const Task_Handle pc_data_tx_tsk_hdl; //Task for UART transmit OK
extern const Task_Handle wait_task_tsk_hdl; //Task for waiting when S is pressed OK

extern const Semaphore_Handle lidar_sample_delay_sem; //Semaphore used for delays between lidar sampling events OK
extern const Semaphore_Handle step_to_sample_sem; //Semaphore used for synchronized stepper and sample tasks OK
extern const Semaphore_Handle sample_to_step_sem; //Semaphore used for synchronized stepper and sample tasks OK
extern const Semaphore_Handle wait_to_lidar_sem; //Semaphore used for synchronized wait and lidar tasks OK
extern const Semaphore_Handle wait_to_step_sem; //Semaphore used for synchronized wait and step tasks OK


Mailbox_Handle distance_to_oled_mailbox; //Mailbox to pass distance to OLED OK
Mailbox_Handle distance_to_pc_mailbox; //Mailbox to pass distance to PC OK
Mailbox_Handle angle_to_pc_mailbox; //Mailbox to pass angle to PC OK
Mailbox_Handle stop_lidar_task_mailbox; //Mailbox to pass "stop" argument to lidar task OK
Mailbox_Handle stop_step_task_mailbox; //Mailbox to pass "stop" argument to step task OK

//task used to stop or start scanning tasks when commands are receieved over UART
void wait_task()
{
    bool stop = false; //intermediate stop to stop the execution of the task OK

    while(1)
    {
        Semaphore_pend(task_wait_sem, BIOS_WAIT_FOREVER); //Pend task wait semaphore forever OK

        //stop lidar task & step task: MP
        stop = true; //stop tasks OK
        Mailbox_post(stop_lidar_task_mailbox, &stop, BIOS_WAIT_FOREVER); //pass stop to lidar task OK
        Semaphore_pend(task_resume_sem, BIOS_WAIT_FOREVER); //wait to be resumed by uart_rx_SWI MP

        //resume step task: MP
        stop = false; //resume tasks OK
        Semaphore_post(wait_to_step_sem); //post wait_to_step OK
        Mailbox_post(stop_step_task_mailbox, &stop, BIOS_WAIT_FOREVER); //mailbox to resume MP

        //resume lidar task MP
        Mailbox_post(stop_lidar_task_mailbox, &stop, BIOS_NO_WAIT); //mailbox to resume MP
        Semaphore_post(wait_to_lidar_sem); //post wait_to_lidar OK
    }
}

void step_task()
{
    bool stop = false; //intermediate stop to stop the execution of the task OK
    uint16_t angle; //angle value OK
    Task_sleep(100U); //wait ~100ms on boot for lidar sensors to stabilize MP
    stepper_zero(); // zero the stepper OK
    Semaphore_post(step_to_sample_sem); //post lidar sample task so it can take initial measurement for EMA filter MP
    Semaphore_pend(sample_to_step_sem, BIOS_WAIT_FOREVER); //wait to be posted by lidar sample task MP
    while(1)
    {
        Mailbox_pend(stop_step_task_mailbox, &stop, BIOS_NO_WAIT); //check for task stopped state OK
        if(stop)
        {
            Semaphore_pend(wait_to_step_sem, BIOS_WAIT_FOREVER); //wait for post OK
            Mailbox_pend(stop_step_task_mailbox, &stop, BIOS_WAIT_FOREVER); //mailbox to resume MP
        }
        angle = stepper_step(10); //take 10 steps MP
        Mailbox_post(angle_to_pc_mailbox, &angle, 10U); //send angle to PC tx task, timeout after 10ms MP


        Semaphore_post(step_to_sample_sem); //post lidar sample task OK
        Semaphore_pend(sample_to_step_sem, BIOS_WAIT_FOREVER); //wait to be posted by lidar sample task OK


    }
}

void lidar_sample_task()
{
   uint16_t distance_buffer[2] = {LIDAR_MAX_MEASUREMENT_RANGE, LIDAR_MAX_MEASUREMENT_RANGE};
   uint16_t ema_window_distance_1[2] = {0, 0};
   uint16_t ema_window_distance_2[2] = {0, 0};
   uint16_t filtered_distance[2] = {LIDAR_MAX_MEASUREMENT_RANGE, LIDAR_MAX_MEASUREMENT_RANGE}; //EMA (exponential moving average) filtered data
   uint16_t filter_alpha = 0.08;
   uint16_t i = 0;
   uint16_t filtered_average_1 = 0;
   uint16_t filtered_average_2 = 0;
   bool stop = false;
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
        for(i = 0; i < 1; i++)
            ema_window_distance_1[i + 1] = ema_window_distance_1[i];
        ema_window_distance_1[0] = filtered_distance[0];

        for(i = 0; i < 1; i++)
             ema_window_distance_2[i + 1] = ema_window_distance_2[i];
        ema_window_distance_2[0] = filtered_distance[1];

        filtered_average_1 = 0;
        filtered_average_2 = 0;

        //calculate averages
        for(i = 0; i < 2; i++)
            filtered_average_1 += ema_window_distance_1[i];

        filtered_average_1 = (uint16_t)((float)filtered_average_1/2.0);

        for(i = 0; i < 2; i++)
                  filtered_average_2 += ema_window_distance_2[i];

        filtered_average_2 = (uint16_t)((float)filtered_average_2/2.0);

        filtered_distance[0] = filtered_average_1;
        filtered_distance[1] = filtered_average_2;

        //send distance to OLED task via mailbox (using NO_WAIT because it is not critical that OLED task receives every sample, it just needs most recent)
        Mailbox_post(distance_to_oled_mailbox, distance_buffer, BIOS_NO_WAIT);
        //send distance to PC sending task timeout if fails after 10ms
        Mailbox_post(distance_to_pc_mailbox, filtered_distance, 10U);

        Mailbox_pend(stop_lidar_task_mailbox, &stop, BIOS_NO_WAIT);

        if(stop)
        {

            Mailbox_post(stop_step_task_mailbox, &stop, BIOS_NO_WAIT);
            Semaphore_post(sample_to_step_sem);
            Semaphore_pend(wait_to_lidar_sem, BIOS_WAIT_FOREVER);  //wait to be posted by wait_task

        }
        else
        {
            Semaphore_post(sample_to_step_sem);
        }

    }



}

void oled_display_task()
{
    uint16_t distance_buffer[2];
    oled_init();

    oled_clear_buffer();
    oled_draw_frame(2, 2, OLED_WIDTH - 2, OLED_HEIGHT - 2, oled_intens_15);
    oled_draw_rectangle(2, 2, OLED_WIDTH - 2, OLED_HEIGHT - 2, oled_intens_2);
    oled_draw_string(100, (20), oled_intens_15, font_f10x16f ,"ZEROING"); //display Zeroing until LED is hit OK
    oled_send_buffer(); //display OK
    Semaphore_pend(zero_sem, BIOS_WAIT_FOREVER); //Wait till semaphore is posted OK


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

void pc_data_tx_task() //send data over UART to PC OK
{
    static uint16_t angle; //angle var OK
    static uint16_t filtered_distance[2]; //distances on both sensors OK

    while(1)
    {
        Mailbox_pend(angle_to_pc_mailbox, &angle, BIOS_WAIT_FOREVER); //Wait for Angle to be received OK
        Mailbox_pend(distance_to_pc_mailbox, filtered_distance, BIOS_WAIT_FOREVER); //wait for distance to be received OK
        GpioDataRegs.GPATOGGLE.bit.GPIO6 |= 1U; //blink LED OK
        uart_tx_str(" %06d %03d %03d\n\r", angle, filtered_distance[0], filtered_distance[1]); //send the string to uart OK
    }
}

/*void idle_fxn(void) //idle fxn OK
{
    GpioDataRegs.GPATOGGLE.bit.GPIO6 |= 1U; //blink LED OK
}*/

/*
 *  ======== main ========
 */
int main()
{ 
    gpio_init(); //initialize GPIO for LED MP
    uart_init(1000000UL); //initialize UART (rate 1000000) OK
    i2c_init(); //initialize I2C MP
    spi_init(4000000UL); //initialize SPI MP
    stepper_init(); //initialize stepper OK
    encoder_init();

    //create mailboxes for communication between TSKs OK
    distance_to_oled_mailbox = Mailbox_create(2 * sizeof(uint16_t), 1, NULL, NULL);
    distance_to_pc_mailbox = Mailbox_create(2 * sizeof(uint16_t), 1, NULL, NULL);
    angle_to_pc_mailbox = Mailbox_create(1 * sizeof(uint16_t), 1, NULL, NULL);
    stop_step_task_mailbox = Mailbox_create(1 * sizeof(bool), 1, NULL, NULL);
    stop_lidar_task_mailbox = Mailbox_create(1 * sizeof(bool), 1, NULL, NULL);
    BIOS_start(); //does not return MP

    return(0);
}


