//in-house libraries
#include "28379D_timer.h"
//standard c libraries
#include <math.h>

bool timer_init(timer_t *timer_hdl)
{

    uint32_t interrupt_num = 0;

    uint32_t period_count = (SysCtl_getClock(DEVICE_OSCSRC_FREQ))/(timer_hdl->freq); //timer cycles = f_cpu/f_desired

    //register ISRs
    switch(timer_hdl->module)
    {
        case CPUTIMER0_BASE:
            interrupt_num = INT_TIMER0;
            Interrupt_register(interrupt_num, timer_hdl->cb_ptr);
            break;

        case CPUTIMER1_BASE:
            interrupt_num = INT_TIMER1;
            Interrupt_register(interrupt_num, timer_hdl->cb_ptr);
            break;

        case CPUTIMER2_BASE:
            interrupt_num = INT_TIMER2;
            Interrupt_register(interrupt_num, timer_hdl->cb_ptr);
            break;

        default:
            return false;

    }




    //put timer in known state
    CPUTimer_stopTimer(timer_hdl->module); //ensure timer is stopped
    CPUTimer_setPeriod(timer_hdl->module, 0xFFFFFFFF); //set period to max possible value
    CPUTimer_setPreScaler(timer_hdl->module, CPUTIMER_CLOCK_PRESCALER_1); //set prescaler/divider to 1

    CPUTimer_setPeriod(timer_hdl->module, period_count); //set period
    CPUTimer_reloadTimerCounter(timer_hdl->module); //reload new period into count register


    CPUTimer_setEmulationMode(timer_hdl->module, CPUTIMER_EMULATIONMODE_STOPATZERO); //set emulation mode

    CPUTimer_enableInterrupt(timer_hdl->module); //enable interrupt for respective timer module

    Interrupt_enable(interrupt_num); //enable respective interrupt in interrupt controller

    CPUTimer_startTimer(timer_hdl->module); //begin counting down

    //enable debug events
    EINT;
    ERTM;

    return true;

}

bool timer_set_freq(timer_t *timer_hdl)
{
    uint32_t interrupt_num = 0;
    uint32_t period_count = (SysCtl_getClock(DEVICE_OSCSRC_FREQ))/(timer_hdl->freq);

    switch(timer_hdl->module)
    {
        case CPUTIMER0_BASE:
            interrupt_num = INT_TIMER0;
            break;

        case CPUTIMER1_BASE:
            interrupt_num = INT_TIMER1;
            break;

        case CPUTIMER2_BASE:
            interrupt_num = INT_TIMER2;
            break;

        default:
            return false;

    }

    Interrupt_disable(interrupt_num); //disable interrupt for respective timer module
    CPUTimer_stopTimer(timer_hdl->module); //stop timer module
    CPUTimer_setPeriod(timer_hdl->module, period_count); //set period
    CPUTimer_reloadTimerCounter(timer_hdl->module); //reload new period into count register
    CPUTimer_startTimer(timer_hdl->module); //begin counting down
    Interrupt_enable(interrupt_num); //re-enable interrupt for respective timer module

    return true;
}









