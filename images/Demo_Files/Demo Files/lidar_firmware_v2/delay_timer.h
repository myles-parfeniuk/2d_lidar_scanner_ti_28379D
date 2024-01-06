#ifndef DELAY_TIMER_H_
#define DELAY_TIMER_H_
/***********************************************************************************
 * 28379D_uart.h
 *
 * A module containing the implementation of a blocking delay for tasks, based
 * off a hardware timer ISR.
 * The timer is configured to have a period of 10us, and to be started and stopped
 * by the user.
 *
 * Author:    Myles Parfeniuk
 * Date:      10/09/2023
 * Modified:  10/20/2023
 *********************************************************************************/
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

extern const Timer_Handle delay_timer_hdl; //timer handle corresponding to periodic timer with period of 10us, that is started and stopped by user

static uint64_t desired_delay = 0; //module scoped global used to pass desired delay to timer ISR
static uint64_t delay_ticks = 0; //module scoped global used to track amount of 10us delay ticks that have passed
static Semaphore_Handle pending_post; //overwritten with semaphore handle passed to delay_task_ticks

//delay a task with a blocking delay for a defined amount of 10us ticks
void delay_task_ticks(uint64_t ticks, Semaphore_Handle semaphore_to_post);
void delay_timer_ISR(UArg arg); //ISR for hardware timer configured in XCONFG



#endif /* DELAY_TIMER_H_ */
