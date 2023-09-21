//in-house libraries
#include "28379D_uart.h"
//standard c libraries
#include <stddef.h>
#include <stdio.h>
#include <math.h>

void uart_init(uint32_t baudrate)
{
    uint32_t baud_reg_val = ((LSP_CLK_FREQ / (baudrate * 8U)) - 1U);;

    EALLOW; //allow writes to protected registers
    CpuSysRegs.PCLKCR7.bit.SCI_A = 1; //enable SCIA module
    //GPIO 43, rx_pin setup
    GpioCtrlRegs.GPBMUX1.bit.GPIO43 |= 0x3U; //mux SCIA_RX to GPIO_43
    GpioCtrlRegs.GPBGMUX1.bit.GPIO43 |= 0x3U;
    GpioCtrlRegs.GPBPUD.bit.GPIO43 = 0; //enable pullup
    GpioCtrlRegs.GPBQSEL1.bit.GPIO43 |= 0x3U; //set asychronous qualification mode (SCI module performs clock sync)

    //GPIO 42, rx_pin setup
    GpioCtrlRegs.GPBMUX1.bit.GPIO42 |= 0x3U;
    GpioCtrlRegs.GPBGMUX1.bit.GPIO42 |= 0x3U;
    GpioCtrlRegs.GPBPUD.bit.GPIO42 = 0;
    GpioCtrlRegs.GPBQSEL1.bit.GPIO42 |= 0x3U;


    SciaRegs.SCIFFRX.bit.RXFFOVRCLR = 1; //clear overflow flag
    //reset tx fifo
    SciaRegs.SCIFFTX.bit.TXFIFORESET = 0;
    SciaRegs.SCIFFTX.bit.TXFIFORESET = 1;
    //reset rx fifo
    SciaRegs.SCIFFRX.bit.RXFIFORESET = 0;
    SciaRegs.SCIFFRX.bit.RXFIFORESET = 1;
    //reset transmit and receive channels
    SciaRegs.SCIFFTX.bit.SCIRST = 0;
    SciaRegs.SCIFFTX.bit.SCIRST = 1;

    SciaRegs.SCIFFTX.bit.SCIFFENA = 1; //enable FIFOs


    //perform soft reset to clear flags
    SciaRegs.SCICTL1.bit.SWRESET = 0;
    SciaRegs.SCICTL1.bit.SWRESET = 1U;

    /*config SCIA module for:
     *
     * - sysclk/4 input clock (default 200MHz/4 = 50MHz)
     * - [baurate]baud
     * - 8 bit word length
     * - one stop bit
     * - no parity
     */
    //disable transmitting & receiving
    SciaRegs.SCICTL1.bit.RXENA = 0;
    SciaRegs.SCICTL1.bit.TXENA = 0;
    //set baud rate
    SciaRegs.SCIHBAUD.all = (baud_reg_val & 0xFF00U) >> 8U;
    SciaRegs.SCILBAUD.all = (baud_reg_val & 0x00FFU);

    SciaRegs.SCICCR.bit.STOPBITS = 0; //set 1 stop bit
    SciaRegs.SCICCR.bit.PARITYENA = 0; //disable parity
    SciaRegs.SCICCR.bit.SCICHAR = 0x7U; //8 bit worth length

    //enable transmitting & receiving
    SciaRegs.SCICTL1.bit.RXENA = 1;
    SciaRegs.SCICTL1.bit.TXENA = 1;

    //perform soft reset to clear flags
    SciaRegs.SCICTL1.bit.SWRESET = 0;
    SciaRegs.SCICTL1.bit.SWRESET = 1U;


    EDIS; //disable writes to protected registers


}

void uart_tx_char(char tx_char)
{
    while(SciaRegs.SCIFFTX.bit.TXFFST != 0)
        {
            ; //wait until tx buffer is empty
        }
        SciaRegs.SCITXBUF.bit.TXDT = tx_char;


}

void uart_tx_str(char *tx_str)
{
    uint16_t i = 0;

    //send chars until NULL
    while(tx_str[i] != NULL)
    {
        uart_tx_char(tx_str[i]);
        i++;
    }

}

void uart_tx_buff(char *tx_buff, uint16_t length)
{
    uint16_t i = 0;

    for(i = 0; i < length; i++)
    {
        uart_tx_char(tx_buff[i]);
    }
}

