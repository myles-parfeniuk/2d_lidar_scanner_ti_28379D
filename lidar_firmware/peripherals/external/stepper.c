/***********************************************************************************
 * stepper.c
 *
 *
 *
 *
 * Author:    Omar Kebedov
 * Date:      12/01/2023
 * Modified:  12/01/2023
 *********************************************************************************/

#include "stepper.h" //in-house include stepper header

void stepper_init(void){

    EALLOW;

    //STEP pin
    GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 0; //denable
    GpioCtrlRegs.GPADIR.bit.GPIO25 = 1; //output
    GpioDataRegs.GPASET.bit.GPIO25 = 0; //initial output

    //DIR pin
    GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 0; //denable
    GpioCtrlRegs.GPADIR.bit.GPIO26 = 1; //output
    GpioDataRegs.GPASET.bit.GPIO26 = 1; //initial output

    //EN pin
    GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 0; //denable
    GpioCtrlRegs.GPADIR.bit.GPIO27 = 1; //output
    GpioDataRegs.GPASET.bit.GPIO27 = 0; //initial output

    //Step Reset (IR sensor)
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0; //config pin as gpio
    GpioCtrlRegs.GPADIR.bit.GPIO0 = 0; //set as input
    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1; //enable pullup

    //Step Reset interrupt setup
    XintRegs.XINT1CR.bit.POLARITY = 0; // Configure XINT1 Falling edge interrupt
    XintRegs.XINT1CR.bit.ENABLE = 1; // Enable XINT1 interrupt
    InputXbarRegs.INPUT4SELECT = 0; // Select XBar input to Input4 output as gpio 0

    initialized = 0;

    EDIS;
}

uint16_t stepper_step(uint16_t steps)
{
    uint16_t i = 0;
    float rads_moved = 0.0;

    for(i = 0; i < steps; i++)
    {
        GpioDataRegs.GPASET.bit.GPIO25 |= 1;
        delay_task_ticks(80U, step_delay_sem);
        GpioDataRegs.GPACLEAR.bit.GPIO25 |= 1;

        if(i != steps - 1)
            delay_task_ticks(80U, step_delay_sem);
        num_steps++;

    }

    rads_moved = 2.0*M_PI*1000.0;
    rads_moved = rads_moved  / (float)run_average_steps;
    rads_moved = rads_moved * (float)num_steps;
    return (uint16_t)(round(rads_moved));
    //return (uint16_t)(round(((2.0*M_PI*1000)/(float)run_window[num_runs])*(float)num_steps));

}

void ir_sensor_ISR(void) //XINT1 ISR, XINT1 is interrupt number 35
{
    EALLOW; //allow access to protected registers
    XintRegs.XINT1CR.bit.ENABLE = 0; // Disable XINT1 interrupt
    if (initialized == 0){
        initialized = 1;
        Semaphore_post (zero_sem);
    }
    else
    {

        run_window[num_runs] = num_steps;
        num_steps = 0;
        num_runs++;

        run_average_steps = ((run_window[0]+run_window[1]+run_window[2]+run_window[3]+run_window[4])/5.0);



        if (num_runs > 5){
            num_runs = 0;
        }
    }
    XintRegs.XINT1CR.bit.ENABLE = 1; // Enable XINT1 interrupt
    EDIS; //disallow access to protected register
}


void stepper_zero(){
    int angle;
    while(!initialized)
    {
        angle = stepper_step(10);
    }
    num_steps = 0;
}
