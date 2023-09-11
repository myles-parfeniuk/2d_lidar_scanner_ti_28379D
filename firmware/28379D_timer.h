/***********************************************************************************
 * 28379D_timer.h
 *
 * A module containing functions allowing for basic operation of TI tm320F828379D
 * controller's hardware timers (CPUTIMER0, CPUTIMER1, & CPUTIMER2).
 *
 *
 *
 * Author:    Myles Parfeniuk
 * Date:      10/09/2023
 * Modified:  10/09/2023
 *********************************************************************************/

#ifndef _28379D_TIMER_H_
#define _28379D_TIMER_H_

//TI c2000Ware libraries
#include "driverlib.h"
#include "device.h"


//structure representing a single CPU timer
typedef struct timer_t
{
    uint32_t module; //hardware module/ timer base, ex. CPUTIMER0_BASE
    uint32_t freq; //frequency of timer (ISR is called upon timer countdown)
    void (*cb_ptr)(void); //pointer to ISR function/callback


} timer_t;


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
* Modified:  10/09/2023
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
* Modified:  10/09/2023
************************************************************************************/
bool timer_set_freq(timer_t *timer_hdl);


#endif /* _28379D_TIMER_H_ */
