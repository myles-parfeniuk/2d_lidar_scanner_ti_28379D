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

    //EN pin
    GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 0; //denable
    GpioCtrlRegs.GPADIR.bit.GPIO26 = 1; //output
    GpioDataRegs.GPASET.bit.GPIO26 = 0; //initial output

    //DIR pin
    GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 0; //denable
    GpioCtrlRegs.GPADIR.bit.GPIO27 = 1; //output
    GpioDataRegs.GPASET.bit.GPIO27 = 1; //initial output

    //Step Reset (IR sensor)
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0; //config pin as gpio
    GpioCtrlRegs.GPADIR.bit.GPIO0 = 0; //set as input
    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1; //enable pullup

    XintRegs.XINT1CR.bit.POLARITY = 0; // Configure XINT1 Falling edge interrupt
    XintRegs.XINT1CR.bit.ENABLE = 1; // Enable XINT1 interrupt
    InputXbarRegs.INPUT4SELECT = 0; // Select XBar input to Input4 output as gpio 0

    EDIS;
}


void stepper_step(void){

    GpioDataRegs.GPASET.bit.GPIO25 |= 1;
    Task_sleep(2U);
    GpioDataRegs.GPACLEAR.bit.GPIO25 |= 1;
    Task_sleep(2U);

}

void ir_sensor_ISR(void) //XINT1 ISR, XINT1 is interrupt number 35
{
    EALLOW; //allow access to protected registers
    XintRegs.XINT1CR.bit.ENABLE = 0; // Disable XINT1 interrupt

    //Do shit

    XintRegs.XINT1CR.bit.ENABLE = 1; // Enable XINT1 interrupt
    EDIS; //disallow access to protected register
}

