#include "encoder.h"

void encoder_init()
{
    EALLOW;

    //encoder channel A
    GpioCtrlRegs.GPBMUX2.bit.GPIO52 = 0; //config pin as gpio
    GpioCtrlRegs.GPBDIR.bit.GPIO52 = 0; //set as input
    GpioCtrlRegs.GPBPUD.bit.GPIO52 = 1; //enable pullup
    //interrupt setup
    XintRegs.XINT2CR.bit.POLARITY = 0; // Configure XINT2 Falling edge interrupt
    XintRegs.XINT2CR.bit.ENABLE = 1; // Enable XINT2 interrupt
    InputXbarRegs.INPUT5SELECT = 52U; // Select XBar input to Input5 as gpio 52

    //encoder channel B
    GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 0; //config pin as gpio
    GpioCtrlRegs.GPBDIR.bit.GPIO41 = 0; //set as input
    GpioCtrlRegs.GPBPUD.bit.GPIO41 = 1; //enable pullup

    //encoder switch
    GpioCtrlRegs.GPBMUX1.bit.GPIO40 = 0; //config pin as gpio
    GpioCtrlRegs.GPBDIR.bit.GPIO40 = 0; //set as input
    GpioCtrlRegs.GPBPUD.bit.GPIO40 = 1; //enable pullup
    //interrupt setup
    XintRegs.XINT3CR.bit.POLARITY = 0; // Configure XINT3 Falling edge interrupt
    XintRegs.XINT3CR.bit.ENABLE = 1; // Enable XINT3 interrupt
    InputXbarRegs.INPUT6SELECT = 40U; // Select XBar input to Input13 output as gpio 40

    EDIS;
}


void encoder_SWI(void)
{
    EALLOW; //allow access to protected registers
    XintRegs.XINT2CR.bit.ENABLE = 1; // enable XINT2 interrupt
    XintRegs.XINT3CR.bit.ENABLE = 1; // enable XINT3 interrupt
    XintRegs.XINT4CR.bit.ENABLE = 1; // enable XINT4 interrupt
    EDIS; //disallow access to protected register
}

void encoder_ch_a_ISR(void)
{
    EALLOW; //allow access to protected registers
    XintRegs.XINT2CR.bit.ENABLE = 0; // disable XINT2 interrupt
    XintRegs.XINT3CR.bit.ENABLE = 0; // disable XINT3 interrupt
    XintRegs.XINT4CR.bit.ENABLE = 0; // disable XINT4 interrupt
    EDIS; //disallow access to protected register

    Swi_post(encoder_swi_hdl);
}

void encoder_switch_ISR(void)
{
    EALLOW; //allow access to protected registers
    XintRegs.XINT2CR.bit.ENABLE = 0; // disable XINT2 interrupt
    XintRegs.XINT3CR.bit.ENABLE = 0; // disable XINT3 interrupt
    XintRegs.XINT4CR.bit.ENABLE = 0; // disable XINT4 interrupt
    EDIS; //disallow access to protected register

    Swi_post(encoder_swi_hdl);
}




