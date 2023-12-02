#include "gpio.h"


void gpio_init(void)
{
//initialize GPIO:
EALLOW; //allow access to protected registers

    //external LED
    GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;
    GpioDataRegs.GPASET.bit.GPIO6 = 1;
EDIS; //disallow access to protected registers
}
