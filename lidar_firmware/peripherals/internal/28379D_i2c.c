//TI includes
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>

//in-house includes
#include "28379D_i2c.h"


void i2c_init()
{
    uint16_t divider = 0;

    EALLOW;

    CpuSysRegs.PCLKCR9.bit.I2C_A = 1; //enable I2CA peripheral

    //gpio initialization
    //SDA - GPIO104
    GpioCtrlRegs.GPDDIR.bit.GPIO104 = 0; //set as input
    GpioCtrlRegs.GPDPUD.bit.GPIO104 = 0; //enable pullup
    GpioCtrlRegs.GPDGMUX1.bit.GPIO104 = 0x00U;
    GpioCtrlRegs.GPDMUX1.bit.GPIO104 = 0x01U; //mux SDAT line of I2CA peripheral to GPIO104
    GpioCtrlRegs.GPDCSEL2.bit.GPIO104 = 0x00; //select CPU0 as master core
    GpioCtrlRegs.GPDQSEL1.bit.GPIO104 |= 0x3U; //set asychronous qualification mode (I2C module doesn't require clock sync)
    //SCLK - GPIO105
    GpioCtrlRegs.GPDDIR.bit.GPIO105 = 0;
    GpioCtrlRegs.GPDPUD.bit.GPIO105 = 0;
    GpioCtrlRegs.GPDGMUX1.bit.GPIO105 = 0x00U;
    GpioCtrlRegs.GPDMUX1.bit.GPIO105 = 0x01U;
    GpioCtrlRegs.GPDCSEL2.bit.GPIO105 = 0x00;
    GpioCtrlRegs.GPDQSEL1.bit.GPIO105 |= 0x3U;

    //I2CA module initialization
    I2caRegs.I2CMDR.bit.IRS = 0; //disable i2c module A for setup

    I2caRegs.I2CPSC.bit.IPSC = (Uint8)((SYS_CLK_FREQ / 10000000UL) - 1UL); //set prescaler to 20 (10MHz input clk)

    //set clock low and high periods for 50% duty cycle and specified frequency (see 20.1.5.1 of t.ref manual)
    divider = (uint16_t)(10000000UL / SCLK_FREQ) - (2UL * D);
    I2caRegs.I2CCLKH = divider/2; //high period
    I2caRegs.I2CCLKL = divider - I2caRegs.I2CCLKH; //low period

    I2caRegs.I2CMDR.bit.MST = 1; //enable master mode
    I2caRegs.I2CMDR.bit.TRX = 1; //enable transmitter mode
    I2caRegs.I2CMDR.bit.DLB = 0; //disable loopback
    I2caRegs.I2CMDR.bit.BC = 0; //8 bits per transmission
    I2caRegs.I2CMDR.bit.XA = 0; //7 bit addressing mode

    //disable interrupts
    I2caRegs.I2CIER.bit.XRDY = 0;
    I2caRegs.I2CIER.bit.RRDY = 0;

    //re-enable module
    I2caRegs.I2CMDR.bit.IRS = 1; //re-enable i2c module A

    EDIS;

}


bool i2c_master_tx(Uint8 dev_addr, const Uint8 *data, uint16_t data_length)
{

    //prep for transmit
    I2caRegs.I2CMDR.bit.MST = 1; //enable master mode
    I2caRegs.I2CMDR.bit.TRX = 1; //enable transmitter mode

    I2caRegs.I2CCNT = data_length; //set amount of bytes to send
    I2caRegs.I2CSAR.bit.SAR = dev_addr; //set target slave address

    //start transaction
    I2caRegs.I2CMDR.bit.STT = 1; //assert start condition
    tx_buffer = data;
    data_tx_length = data_length;

    //enable transmit and NACK interrupt
    I2caRegs.I2CIER.bit.XRDY =  1;
    I2caRegs.I2CIER.bit.NACK = 1;

    //wait for SWI to post semaphore
    if(Semaphore_pend(i2c_tx_sem, I2C_TIMEOUT_MS))
    {
        if(nack_detected)
            return false; //fail transaction, slave did not ACK transmission
        else
            return true;
    }
    else
        return false; //fail transaction, timed out

}

bool i2c_master_rx(Uint8 dev_addr, Uint8 *data, uint16_t data_length)
{
    uint16_t i = 0;

    //prep for receive
    I2caRegs.I2CMDR.bit.MST = 1; //enable master mode
    I2caRegs.I2CMDR.bit.TRX = 0; //enable receiver mode

    I2caRegs.I2CCNT = data_length; //set amount of bytes to send
    I2caRegs.I2CSAR.bit.SAR = dev_addr; //set target slave address

    //start transaction
    I2caRegs.I2CMDR.bit.STT = 1; //assert start condition
    rx_buffer = data;
    data_rx_length = data_length;

    //enable receive and NACK interrupt
    I2caRegs.I2CIER.bit.RRDY =  1;
    I2caRegs.I2CIER.bit.NACK = 1;

    if(Semaphore_pend(i2c_rx_sem, I2C_TIMEOUT_MS))
    {
        if(nack_detected)
            return false;
        else
            return true;
    }
    else
        return false; //fail transaction, timed out


    return true;
}

bool i2c_nack_check()
{
    if(I2caRegs.I2CSTR.bit.NACK) //check for NACK
    {

       //clear NACK and data ready flags
       I2caRegs.I2CSTR.bit.NACK = 1;
       I2caRegs.I2CSTR.bit.XRDY = 1;
       I2caRegs.I2CMDR.bit.STP = 1;//assert stop condition

       return true;
    }

    return false;
}

void i2c_tx_handler_SWI(void)
{
    static uint16_t i = 0;

    //check for slave NACK
    nack_detected = i2c_nack_check();

    if(nack_detected)
        Semaphore_post(i2c_tx_sem); //slave failed to ack, post sem and fail transaction
    else
    {
        I2caRegs.I2CDXR.bit.DATA = tx_buffer[i];
        i++;
    }

    if(i >= data_tx_length)
    {
        i = 0;
        //end successful transaction
        I2caRegs.I2CMDR.bit.STP = 1;//assert stop condition
        Semaphore_post(i2c_tx_sem); //post sem, transaction succeeded
    }
    else
    {
        //re-enable interrupts for next byte
        I2caRegs.I2CIER.bit.XRDY =  1;
        I2caRegs.I2CIER.bit.NACK = 1;
    }



}

void i2c_rx_handler_SWI(void)
{
    static uint16_t i = 0;

    //check for slave NACK
    nack_detected = i2c_nack_check();

    if(nack_detected)
        Semaphore_post(i2c_rx_sem); //slave failed to ack, post sem and fail transaction
    else
    {
        //master must assert NACK on second to last byte (see tech ref manual), this NACK tells the slave the master is done receiving
        if(i == (data_rx_length - 2))
            I2caRegs.I2CMDR.bit.NACKMOD = 1; //assert NACK

        rx_buffer[i] = I2caRegs.I2CDRR.bit.DATA;
        i++;
    }

    if(i >= data_rx_length)
    {
        i = 0;
        //end successful transaction
        I2caRegs.I2CMDR.bit.STP = 1;//assert stop condition
        Semaphore_post(i2c_rx_sem); //post sem, transaction succeeded
    }
    else
    {
        //enable receive and NACK interrupt
        I2caRegs.I2CIER.bit.RRDY =  1;
        I2caRegs.I2CIER.bit.NACK = 1;
    }

}

void i2c_handler_ISR(void)
{
    EALLOW; //allow access to protected registers

    if(TX_MODE)
    {
        //disable all used i2c interrupts
        I2caRegs.I2CIER.bit.NACK = 0;
        I2caRegs.I2CIER.bit.XRDY = 0;
        Swi_post(i2c_tx_swi_hdl);

    }
    else if(RX_MODE)
    {
        //disable all used i2c interrupts
        I2caRegs.I2CIER.bit.NACK = 0;
        I2caRegs.I2CIER.bit.RRDY = 0;
        Swi_post(i2c_rx_swi_hdl);
    }


    EDIS; //disallow access to protected register
}

