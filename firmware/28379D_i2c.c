//in-house libraries
#include "28379D_i2c.h"

void i2c_init()
{
    EALLOW;

    CpuSysRegs.PCLKCR9.bit.I2C_A = 1; //enable I2CA peripheral

    //gpio initialization
    //SDA - GPIO104
    GpioCtrlRegs.GPDDIR.bit.GPIO104 = 0; //set as input
    GpioCtrlRegs.GPDPUD.bit.GPIO104 = 0; //enable pullup
    GpioCtrlRegs.GPDMUX1.bit.GPIO104 = 0x01U; //mux SDAT line of I2CA peripheral to GPIO104
    GpioCtrlRegs.GPDGMUX1.bit.GPIO104 = 0x00U;
    GpioCtrlRegs.GPDCSEL2.bit.GPIO104 = 0x00; //select CPU0 as master core
    GpioCtrlRegs.GPDQSEL1.bit.GPIO104 |= 0x3U; //set asychronous qualification mode (SCI module performs clock sync)
    //SCLK - GPIO105
    GpioCtrlRegs.GPDDIR.bit.GPIO105 = 0;
    GpioCtrlRegs.GPDPUD.bit.GPIO105 = 0;
    GpioCtrlRegs.GPDMUX1.bit.GPIO105 = 0x01U;
    GpioCtrlRegs.GPDGMUX1.bit.GPIO105 = 0x00U;
    GpioCtrlRegs.GPDCSEL2.bit.GPIO105 = 0x00;
    GpioCtrlRegs.GPDQSEL1.bit.GPIO105 |= 0x3U;

    //I2CA module initialization
    I2caRegs.I2CMDR.bit.IRS = 0; //disable i2c module A for setup
    I2caRegs.I2CPSC.bit.IPSC = 0xAU; //set max prescaler (16)
    //set clock low and high periods
    I2caRegs.I2CCLKH = ((10000000U / 400000U) - (2U * 5))/2; //high period
    I2caRegs.I2CCLKL = ((10000000U / 400000U) - (2U * 5)) - I2caRegs.I2CCLKH; //low period
    I2caRegs.I2CMDR.bit.MST = 1; //enable master mode
    I2caRegs.I2CMDR.bit.TRX = 1; //enable transmitter mode
    I2caRegs.I2CMDR.bit.DLB = 0; //disable loopback
    I2caRegs.I2CMDR.bit.BC = 0; //8 bits per transmission
    I2caRegs.I2CMDR.bit.XA = 0; //7 bit addressing mode
    //disable fifo
    I2caRegs.I2CFFTX.bit.I2CFFEN = 0;
    I2caRegs.I2CFFTX.bit.TXFFRST = 0;
    I2caRegs.I2CFFRX.bit.RXFFRST = 0;

    //re-enable module
    I2caRegs.I2CMDR.bit.IRS = 1; //re-enable i2c module A

    EDIS;

}


bool i2c_master_tx(char dev_addr, char *data, uint16_t data_length)
{

    uint16_t i = 0;


    I2caRegs.I2CMDR.bit.MST = 1; //enable master mode
    I2caRegs.I2CMDR.bit.TRX = 1; //enable transmitter mode

    I2caRegs.I2CCNT = data_length; //set amount of bytes to send
    I2caRegs.I2CSAR.bit.SAR = dev_addr; //set target slave address

    I2caRegs.I2CMDR.bit.STT = 1; //assert start condition




    for(i = 0; i < data_length; i++)
    {

        while (!I2caRegs.I2CSTR.bit.XRDY) {
            // Wait for the transmit ready flag

            if(i2c_nack_check())
            {
                return false;
            }
        }


        I2caRegs.I2CDXR.bit.DATA = data[i];

    }



    I2caRegs.I2CMDR.bit.STP = 1;//assert stop condition

    return true;


}

bool i2c_master_rx(char dev_addr, char *data, uint16_t data_length)
{
    uint16_t i = 0;

    i2c_init();

    I2caRegs.I2CMDR.bit.MST = 1; //enable master mode
    I2caRegs.I2CMDR.bit.TRX = 0; //enable receiver mode

    I2caRegs.I2CCNT = data_length; //set amount of bytes to send
    I2caRegs.I2CSAR.bit.SAR = dev_addr; //set target slave address

    I2caRegs.I2CMDR.bit.STT = 1; //assert start condition


    for (i = 0; i < data_length; i++)
    {
        // Wait until data is received
        while (!I2caRegs.I2CSTR.bit.RRDY)
        {
            // Check for NACK
            if (i2c_nack_check())
            {
                return false;
            }

        }

        // If it's the last byte, send NACK before stopping
        if (i == (data_length - 2))
        {
            I2caRegs.I2CMDR.bit.NACKMOD = 1;
        }

        // Read the received data
        data[i] = (char)I2caRegs.I2CDRR.bit.DATA;

    }

    I2caRegs.I2CMDR.bit.STP = 1;//assert stop condition

    return true;
}

bool i2c_nack_check()
{
    if(I2caRegs.I2CSTR.bit.NACK) //check for nack
    {

               //clear nack and data ready flags
               I2caRegs.I2CSTR.bit.NACK = 1;
               I2caRegs.I2CSTR.bit.XRDY = 1;
               I2caRegs.I2CMDR.bit.STP = 1;//assert stop condition

               return true;
    }

    return false;
}
