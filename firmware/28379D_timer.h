/*
 * gpio.h
 *
 *  Created on: Sep. 20, 2023
 *      Author: myles
 */

#ifndef _28379D_TIMER_H_
#define _28379D_TIMER_H_
#include <Headers/F2837xD_device.h>


#define SYS_CLK_FREQ 200000000U
#define CPU_TIMER_PRESC 0U

//structure representing a single CPU timer
typedef struct timer_t
{
    uint32_t module; //hardware module/ timer base, ex. CPUTIMER0_BASE
    uint32_t freq; //frequency of timer (ISR is called upon timer countdown)
    void (*cb_ptr)(void); //pointer to ISR function/callback


} timer_t;

enum cpu_timer_modules {CPU_TIM_0, CPU_TIM_1};


/***********************************************************************************
*
* timer_init
*
* Initializes a hardware timer according the params within the supplied timer handle.
*
*
* arguments:
*        timer_hdl - a pointer to a timer handle struct containing the initialization arguments
*
* return:   true if successful initialization, false if otherwise
*
* Author:    Myles Parfeniuk
* Date:      10/09/2023
* Modified:  10/20/2023
************************************************************************************/
bool timer_init(timer_t *timer_hdl);


/***********************************************************************************
*
* timer_init
*
* Modifies the frequency of a hardware timer according the params within the supplied timer handle.
* Stops the timer to change the frequency, reloads new timer period.
*
*
* arguments:
*        timer_hdl - a pointer to a timer handle struct containing the freq arguments
*
* return:   true if successful freq switch, false if otherwise
*
* Author:    Myles Parfeniuk
* Date:      10/09/2023
* Modified:  10/20/2023
************************************************************************************/
bool timer_set_freq(timer_t *timer_hdl);


static void cpu_timer_0_init(uint32_t period_count);
static void cpu_timer_1_init(uint32_t period_count);


#endif /*_28379D_TIMER_H_*/
