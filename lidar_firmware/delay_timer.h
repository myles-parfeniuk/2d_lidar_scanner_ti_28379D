#ifndef DELAY_TIMER_H_
#define DELAY_TIMER_H_

//TI includes
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
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/hal/Timer.h>

extern const Timer_Handle delay_timer_hdl;

static uint64_t desired_delay = 0;
static uint64_t delay_ticks = 0;
static Semaphore_Handle pending_post;


uint64_t get_delay_ticks();
void delay_task_ticks(uint64_t ticks, Semaphore_Handle semaphore_to_post);
void delay_tick_reset_SWI(void);
void delay_timer_ISR(UArg arg);



#endif /* DELAY_TIMER_H_ */
