
#ifndef _28379D_SPI_H_
#define _28379D_SPI_H_

//TI includes
#include <Headers/F2837xD_device.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

//constants
#define LSP_CLK_FREQS 50000000UL
//macros
#define DE_ASSRT_CS GpioDataRegs.GPDSET.bit.GPIO123 = 1
#define ASSRT_CS    GpioDataRegs.GPDCLEAR.bit.GPIO123 = 1

#define SPI_TIMEOUT_MS 5U //amount of time to wait for tx flag to be asserted before failing transaction

extern const Semaphore_Handle spi_tx_sem; //Semaphore used for synchronization of SPI transmitting events

/***********************************************************************************
*
* spi_init
*
* Initializes SPIA module with specified baud rate.
*
*
* arguments:
*           baud_rate - Desired baud rate in Hz
*
* return:   n/a
*
* Author:    Myles Parfeniuk
* Date:      10/09/2023
* Modified:  12/02/2023
************************************************************************************/
void spi_init(uint64_t baud_rate);

/***********************************************************************************
*
* spi_transmit
*
* Transmits a data buffer with a specified length in bytes via SPI bus on MOSI (master out slave in)
* pin. Determines when byte is finished transmitting by assertion of receive interrupt (SPIRXBUF has 0 bits shifted in
* at the same time transmitting bits are shifted out, it will be full upon transmission completion).
*
*
* arguments:
*           data - Pointer to first element of data buffer.
*           length - Total amount of bytes to send.
*
* return:   True if transaction succeeded, false if receive interrupt was never asserted.
*
* Author:    Myles Parfeniuk
* Date:      10/09/2023
* Modified:  12/02/2023
************************************************************************************/
bool spi_transmit(Uint8 *data, uint16_t length);

/***********************************************************************************
*
* spi_handler_ISR
*
* This handler is used to take action with the INT_FLAG within the SPISTS register
* has been asserted due to the SPIRXBUF register being full.
*
* This occurs when the transmission of a single byte has completed (SPIRXBUF has 0 bits shifted in at the same time
* transmitting bits are shifted out, it will be full upon transmission completion).
*
* The handler clears the INT_FLAG by performing a dummy read of the SPIRXBUF register, it then posts a semaphore leading
* to the continuation of the execution of the spi_transmit function.
*
*
* arguments:
*           n/a
*
* return:   n/a
*
* Author:    Myles Parfeniuk
* Date:      10/09/2023
* Modified:  12/02/2023
************************************************************************************/
void spi_handler_ISR(void);


#endif /* _28379D_SPI_H_ */
