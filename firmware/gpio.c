#include "gpio.h"


void init_gpio(void)
{
//initialize GPIO:
EALLOW; //allow access to protected registers

    //configure D10 (blue LED)
    GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0; //set pin as gpio
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1; //set gpio as output
    GpioDataRegs.GPASET.bit.GPIO31 = 1; //initialize output value to "1"

    GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;
    GpioDataRegs.GPASET.bit.GPIO6 = 1;


EDIS; //disallow access to protected registers
}
