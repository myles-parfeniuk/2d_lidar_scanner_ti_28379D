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

    initialized = 0; //not initialized yet, thus need to take steps till ISR

    EDIS; //EALLOW
}

uint16_t stepper_step(uint16_t steps) //take a step
{
    uint16_t i = 0; //intermediate val for "for"
    float rads_moved = 0.0; //how many rads are moved - to be converted to int as mailbox may not support floats

    for(i = 0; i < steps; i++) //toggle as many times as specified
    {
        GpioDataRegs.GPASET.bit.GPIO25 |= 1;//toggle
        delay_task_ticks(80U, step_delay_sem);//wait - made by MP
        GpioDataRegs.GPACLEAR.bit.GPIO25 |= 1;//toggle

        if(i != steps - 1) //wait after toggle
            delay_task_ticks(80U, step_delay_sem); //wait again, avoids the jerk of the motor due to staying low while lidar is taking a sample
        num_steps++; //make a step

    }

    rads_moved = 2.0*M_PI*1000.0; //calculate angle in rads
    rads_moved = rads_moved  / (float)run_average_steps;
    rads_moved = rads_moved * (float)num_steps;
    return (uint16_t)(round(rads_moved)); //pass the angle back

}

void ir_sensor_ISR(void) //XINT1 ISR, XINT1 is interrupt number 35
{
    EALLOW; //allow access to protected registers
    XintRegs.XINT1CR.bit.ENABLE = 0; // Disable XINT1 interrupt
    if (initialized == 0){
        initialized = 1; //once sensor is hit - initialize
        Semaphore_post (zero_sem); //zeroed
    }
    else
    {

        run_window[num_runs] = num_steps; //assign the number of steps into the window for averaging
        num_steps = 0; //make it 0
        num_runs++; //shift to the next numrun

        run_average_steps = ((run_window[0]+run_window[1]+run_window[2]+run_window[3]+run_window[4])/5.0); //average number of steps



        if (num_runs > 5){
            num_runs = 0; //reset when hit 5
        }
    }
    XintRegs.XINT1CR.bit.ENABLE = 1; // Enable XINT1 interrupt
    EDIS; //EALLOW
}


void stepper_zero(){ //take steps until initialized
    int angle;
    while(!initialized)
    {
        angle = stepper_step(10);
    }
    num_steps = 0;
}
