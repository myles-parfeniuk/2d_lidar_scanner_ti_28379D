//in-house includes
#include "28379D_uart.h"

//C standard library includes
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>

void uart_init(uint32_t baud_rate)
{
    uint32_t baud_reg_val = ((LSP_CLK_FREQ / (baud_rate * 8U)) - 1U);

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

    SciaRegs.SCIFFRX.bit.RXFFIL = 1; //set RXFFINT level (RXFFINT asserted when receive FIFO contains one or more words)
    SciaRegs.SCIFFRX.bit.RXFFIENA = 1; //enable FIFO receive interrupt (RXFFINT)

    EDIS; //disable writes to protected registers


}

void uart_tx_char(char tx_char)
{
    SciaRegs.SCIFFTX.bit.TXFFIL = 0; //set TXFFINT level (TXFFINT asserted when Transmit FIFO is empty)
    SciaRegs.SCIFFTX.bit.TXFFIENA = 1; //enable FIFO transmit interrupt (TXFFINT)

    //wait for HWI to post send semaphore
    Semaphore_pend(uart_tx_sem, 5U);

    //write next byte to transmit
    SciaRegs.SCITXBUF.bit.TXDT = tx_char;


}

void uart_tx_str(const char *format, ...)
{
    char *buffer;
    uint16_t length;
    va_list args;
    uint16_t i = 0;

    va_start(args, format); //initialize va_list (used to dynamically parse arguments)

    length = vsnprintf(NULL, 0, format, args);     //calculate the length required for the formatted string
    buffer = (char *)malloc(length + 1);           //allocate a buffer for formatted string


    vsnprintf(buffer, length + 1, format, args); //write string to buffer

    //while string hasn't reached end (NULL key) send characters
    while (buffer[i] != '\0')
    {
        uart_tx_char(buffer[i++]); //send current char
    }

    //cleanup
    free(buffer); //de-allocate buffer
    va_end(args); //cleanup va_list
}

void uart_tx_buff(char *tx_buff, uint16_t length)
{
    uint16_t i = 0;

    for(i = 0; i < length; i++)
    {
        uart_tx_char(tx_buff[i]);
    }
}

void uart_rx_SWI(void)
{
    static char rx_str[100];
    static Uint8 index = 0;

    //only receive up to 100 chars, reset index otherwise
    if(index >= MAX_RX_STR_BYTES)
        index = 0;

    rx_str[index++] = SciaRegs.SCIRXBUF.bit.SAR; //read the received character

    SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1; //clear RXFFINT flag
    SciaRegs.SCIFFRX.bit.RXFFIENA = 1; //re-enable FIFO receive interrupt (RXFFINT)

    //detect EOL delimiters (in this case, enter or new line keys)
    if(rx_str[index-1] == '\r' || rx_str[index-1] == '\n')
    {
        rx_str[index-1] = '\0'; //add null key to terminate string
        index  = 0; //reset index

        //post any SWIs or semaphores here
    }
}

void uart_tx_handler_ISR(void)
{
    SciaRegs.SCIFFTX.bit.TXFFIENA = 0; //disable FIFO transmit interrupt (TXFFINT)
    Semaphore_post(uart_tx_sem);
}

void uart_rx_handler_ISR(void)
{
   SciaRegs.SCIFFRX.bit.RXFFIENA = 0; //disable FIFO receive interrupt (RXFFINT)
   Swi_post(uart_rx_swi_hdl);
}
