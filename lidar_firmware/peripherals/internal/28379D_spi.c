#include "28379D_spi.h"

void spi_init()
{

    EALLOW;

    CpuSysRegs.PCLKCR8.bit.SPI_A = 1; //enable SPIA peripheral clock

    //gpio initialization

    //MOSI (MOMI for 3-wire) - GPIO58 (pin 15)
    GpioCtrlRegs.GPBDIR.bit.GPIO58 = 1U; //set as output (only ever sending to slave)
    GpioCtrlRegs.GPBPUD.bit.GPIO58 = 0; //enable pullup
    //mux MOSI line of SPIA peripheral to GPIO58
    GpioCtrlRegs.GPBGMUX2.bit.GPIO58 = 0x3U;
    GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 0x3U;
    GpioCtrlRegs.GPBCSEL4.bit.GPIO58 = 0x00; //select CPU0 as master core
    GpioCtrlRegs.GPBQSEL2.bit.GPIO58 = 0x3U; //set asynchronous qualification mode (SPIA module doesn't require clock sync)

    //CS (chip select) - GPIO123 (pin 18) (Not using SPISTE)
    GpioCtrlRegs.GPDDIR.bit.GPIO123 = 1U; //set as output
    GpioCtrlRegs.GPDPUD.bit.GPIO123 = 0; //enable pullup
    GpioDataRegs.GPDSET.bit.GPIO123 = 1; //set CS as initially high

    //If using SPISTE:
    /*GpioCtrlRegs.GPBDIR.bit.GPIO61 = 1U; //set as output
    GpioCtrlRegs.GPBPUD.bit.GPIO61 = 0; //enable pullup
    //mux CS line of SPIA peripheral to GPIO61
    GpioCtrlRegs.GPBGMUX2.bit.GPIO61 = 0x3U;
    GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 0x3U;
    GpioCtrlRegs.GPBCSEL4.bit.GPIO61 = 0x00; //select CPU0 as master core
    GpioCtrlRegs.GPBQSEL2.bit.GPIO61 = 0x3U; //set asynchronous qualification mode (SPIA module doesn't require clock sync)*/

    //SCLK - GPIO60
    GpioCtrlRegs.GPBDIR.bit.GPIO60 = 1; //set as output
    GpioCtrlRegs.GPBPUD.bit.GPIO60 = 0; //enable pullup
    //mux SCLK line of SPIA peripheral to GPIO60
    GpioCtrlRegs.GPBGMUX2.bit.GPIO60 = 0x3U;
    GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 0x3U;
    GpioCtrlRegs.GPBCSEL4.bit.GPIO60 = 0x00; //select CPU0 as master core
    GpioCtrlRegs.GPBQSEL2.bit.GPIO60 = 0x3U; //set asynchronous qualification mode (SPIA module doesn't require clock sync)

    //SPIA module initialization
    SpiaRegs.SPICCR.bit.SPISWRESET = 0; //disable SPIA module for setup
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 0; //clock polarity mode 0 (SCLK idles low)
    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1; //enable master mode
    SpiaRegs.SPICTL.bit.CLK_PHASE = 1; //MOSI changes on falling edges of SCLK
    SpiaRegs.SPICCR.bit.HS_MODE = 0; //disable high speed mode
    SpiaRegs.SPIPRI.bit.TRIWIRE = 1; //enable 3 wire mode

    //set baud rate
    SpiaRegs.SPIBRR.bit.SPI_BIT_RATE = (LSP_CLK_FREQS/SPI_BAUD_RATE) - 1; //calculate correct divider (see 18.5.2.4)

    SpiaRegs.SPICCR.bit.SPICHAR = 0x7U;

    SpiaRegs.SPISTS.bit.OVERRUN_FLAG = 1; //clear overrun flag in case it's asserted
    SpiaRegs.SPIRXBUF; //clear INT flag by reading SPIRXBUF in case it's asserted

    SpiaRegs.SPICCR.bit.SPISWRESET = 1; //take SPIA module out of reset

    EDIS;
}

void spi_transmit(Uint8 *data, uint16_t length)
{
    uint16_t i = 0;
    uint16_t dummy_read = 0;

    ASSRT_CS;
    //for(i = 0; i < 56; i++);//delay for approx 280ns

    SpiaRegs.SPICTL.bit.TALK = 1;

    for(i = 0; i < length; i++)
    {
        SpiaRegs.SPITXBUF = (data[i] << 8);
        while(SpiaRegs.SPISTS.bit.INT_FLAG != 1) {}
        dummy_read = SpiaRegs.SPIRXBUF;


    }

    for(i = 0; i < 50; i++); //delay
    DE_ASSRT_CS;
}
