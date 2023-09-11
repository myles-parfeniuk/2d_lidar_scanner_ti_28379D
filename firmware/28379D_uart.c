//in-house libraries
#include "28379D_uart.h"
//standard c libraries
#include <stddef.h>
#include <stdio.h>
#include <math.h>

void uart_init(uint32_t baudrate)
{

    EALLOW; //allow writes to protected registers

    //rx pin setup for gpio 43 (see dev board schematic)
    GPIO_setPinConfig(GPIO_43_SCIRXDA);
    GPIO_setPadConfig(43, GPIO_PIN_TYPE_STD | GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(43, GPIO_QUAL_ASYNC); //set asynchronous qualification (input synchronization performed by SCI module itself)

    //tx pin setup for gpio 42 (see dev board schematic)
    GPIO_setPinConfig(GPIO_42_SCITXDA);
    GPIO_setPadConfig(42, GPIO_PIN_TYPE_STD | GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(42, GPIO_QUAL_ASYNC);

    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_RXFF | SCI_INT_TXFF | SCI_INT_FE | SCI_INT_OE | SCI_INT_PE | SCI_INT_RXERR | SCI_INT_RXRDY_BRKDT | SCI_INT_TXRDY); //clear all relevant interrupts for SCIA module
    SCI_clearOverflowStatus(SCIA_BASE); //clear overflow flags
    SCI_resetTxFIFO(SCIA_BASE); //reset sending buffer
    SCI_resetRxFIFO(SCIA_BASE); //reset receiving buffer
    SCI_resetChannels(SCIA_BASE); //reset transmit and receive channels

    /*config SCIA module for:
     *
     * - sysclk/4 input clock (default 200MHz/4 = 50MHz)
     * - [baurate]baud
     * - 8 bit word length
     * - one stop bit
     * - no parity
     */
    SCI_setConfig(SCIA_BASE, SysCtl_getLowSpeedClock(DEVICE_OSCSRC_FREQ), baudrate, (SCI_CONFIG_WLEN_8|SCI_CONFIG_STOP_ONE|SCI_CONFIG_PAR_NONE));

    SCI_performSoftwareReset(SCIA_BASE); //soft reset SCI module (clears all flags but doesn't affect config bits)
    SCI_setFIFOInterruptLevel(SCIA_BASE, SCI_FIFO_TX0, SCI_FIFO_RX0); //set FIFO level at which interrupts are generated to empty
    SCI_enableFIFO(SCIA_BASE);
    SCI_enableModule(SCIA_BASE);


    EDIS; //disable writes to protected registers


}

void uart_tx_char(char tx_char)
{
    while(SCI_getTxFIFOStatus(SCIA_BASE) != SCI_FIFO_TX0)
        {
            ; //wait until tx buffer is empty
        }
        SCI_writeCharNonBlocking(SCIA_BASE, (uint16_t)tx_char);


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

