#include "delay_timer.h"


//delays in ticks of 10us
void delay_task_ticks(uint64_t ticks, Semaphore_Handle semaphore_to_post)
{
  delay_ticks = 0; //reset delay ticks
  pending_post = semaphore_to_post; //set the local scope semaphore to the semaphore that should be posted on delay timeout
  desired_delay = ticks; //set the desired delay to the passed ticks
  Timer_start(delay_timer_hdl); //start the hardware timer
  Semaphore_pend(pending_post, BIOS_WAIT_FOREVER); //wait until pended by delay_timer_ISR
  Timer_stop(delay_timer_hdl); //stop delay timer before returning from function
}


//this ISR is entered every 10us if it is enabled (see XCONFG timer setup)
void delay_timer_ISR(UArg arg)
{

        delay_ticks ++;

        if(delay_ticks > desired_delay)
        {
            delay_ticks = 0;
            Semaphore_post(pending_post); //post semaphore passed by call to delay_task_ticks
        }
}




