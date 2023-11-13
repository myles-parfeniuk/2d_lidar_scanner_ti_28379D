
#ifndef _28379D_SPI_H_
#define _28379D_SPI_H_

//TI includes
#include <Headers/F2837xD_device.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

//constants
#define LSP_CLK_FREQS 50000000UL
#define SPI_BAUD_RATE 4000000UL

//macros
#define DE_ASSRT_CS GpioDataRegs.GPDSET.bit.GPIO123 = 1
#define ASSRT_CS    GpioDataRegs.GPDCLEAR.bit.GPIO123 = 1


void spi_init();
void spi_transmit(Uint8 *data, uint16_t length);



#endif /* _28379D_SPI_H_ */
