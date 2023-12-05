#include "delay_timer.h"


uint64_t get_delay_ticks()
{
    return delay_ticks;
}

//delays in ticks of 10us
void delay_task_ticks(uint64_t ticks, Semaphore_Handle semaphore_to_post)
{
  delay_ticks = 0;
  pending_post = semaphore_to_post;
  desired_delay = ticks;
  Timer_start(delay_timer_hdl);
  Semaphore_pend(pending_post, BIOS_WAIT_FOREVER);
  Timer_stop(delay_timer_hdl);
}


void delay_timer_ISR(UArg arg)
{

        delay_ticks ++;

        if(delay_ticks > desired_delay)
        {
            delay_ticks = 0;
            Semaphore_post(pending_post);
        }
}




