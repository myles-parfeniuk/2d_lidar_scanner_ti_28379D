#include "28379D_timer.h"


bool timer_init(timer_t *timer_hdl)
{

    uint32_t period_count = SYS_CLK_FREQ/timer_hdl->freq; //timer cycles = f_cpu/f_desired


    EALLOW; //allow access to protected registers

    switch(timer_hdl->module)
    {
        case CPU_TIM_0:
            PieVectTable.TIMER0_INT = timer_hdl->cb_ptr;
            cpu_timer_0_init(period_count);
            break;

        case CPU_TIM_1:
            PieVectTable.TIMER1_INT = timer_hdl->cb_ptr;
            cpu_timer_1_init(period_count);
            break;

        default:
            return false;

    }


    EDIS; //disallow access to protected registers

    return true;
}


bool timer_set_freq(timer_t *timer_hdl)
{
    uint32_t period_count = SYS_CLK_FREQ/timer_hdl->freq; //timer cycles = f_cpu/f_desired
    switch(timer_hdl->module)
    {
        case CPU_TIM_0:
            CpuTimer0Regs.PRD.all = period_count; //set period
            CpuTimer0Regs.TCR.bit.TRB|= 1U; //reload period count register
            break;

        case CPU_TIM_1:
            CpuTimer1Regs.PRD.all = period_count; //set period
            CpuTimer1Regs.TCR.bit.TRB|= 1U; //reload period count register
            break;

        default:
            EDIS;
            return false;

    }
    return true;
}

static void cpu_timer_0_init(uint32_t period_count)
{
    //bring timer to known state
    CpuTimer0Regs.TCR.bit.TSS|= 1U; //stop timer
    CpuTimer0Regs.PRD.all = 0xFFFFFFFF; //set period to max possible value
    //set prescaler/divider to 1
    CpuTimer0Regs.TPRH.all = (CPU_TIMER_PRESC >> 8U);
    CpuTimer0Regs.TPR.all = (CPU_TIMER_PRESC & 0xFFU);
    //load calculated period count
    CpuTimer0Regs.PRD.all = period_count;
    //reload new period into count register
    CpuTimer0Regs.TCR.bit.TRB|= 1U;

    //set emulation mode to stop at 0
    CpuTimer0Regs.TCR.bit.FREE &= ~1U;
    CpuTimer0Regs.TCR.bit.SOFT |= 1U;

    //clear local interrupt flag
    CpuTimer0Regs.TCR.bit.TIF = 1;
    //enable local interrupt for timer module
    CpuTimer0Regs.TCR.bit.TIE |= 1U;

    DINT;
    PieCtrlRegs.PIECTRL.bit.ENPIE |= 1U;
    PieCtrlRegs.PIEIER1.bit.INTx7 |= 1U;
    IER |= 0x0001;
    EINT;

    CpuTimer0Regs.TCR.bit.TSS &= ~1U; //start timer


}



static void cpu_timer_1_init(uint32_t period_count)
{
    //bring timer to known state
    CpuTimer1Regs.TCR.bit.TSS|= 1U; //stop timer
    CpuTimer1Regs.PRD.all = 0xFFFFFFFF; //set period to max possible value
    //set prescaler/divider to 1
    CpuTimer1Regs.TPRH.all = (CPU_TIMER_PRESC >> 8U);
    CpuTimer1Regs.TPR.all = (CPU_TIMER_PRESC & 0xFFU);
    //load calculated period count
    CpuTimer1Regs.PRD.all = period_count;
    //reload new period into count register
    CpuTimer1Regs.TCR.bit.TRB|= 1U;

    //set emulation mode to stop at 0
    CpuTimer1Regs.TCR.bit.FREE &= ~1U;
    CpuTimer1Regs.TCR.bit.SOFT |= 1U;

    //enable local interrupt for timer module
    CpuTimer1Regs.TCR.bit.TIE |= 1U;

    DINT;
    IER |= (1U << 12U);
    EINT;

    CpuTimer1Regs.TCR.bit.TSS &= ~1U; //start timer
}


