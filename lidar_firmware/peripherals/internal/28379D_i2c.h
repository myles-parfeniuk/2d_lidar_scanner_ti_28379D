/***********************************************************************************
 * 28379D_i2c.h
 *
 * A module containing functions allowing for basic I2C communications with
 * the I2CA peripheral of the TI tm320F828379D controller.
 *
 * Author:    Myles Parfeniuk
 * Date:      10/09/2023
 * Modified:  12/02/2023
 *********************************************************************************/
#ifndef _28379D_I2C_H_
#define _28379D_I2C_H_

//C standard library includes
#include <ctype.h>

//TI includes
#include <Headers/F2837xD_device.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Swi.h>


#define SYS_CLK_FREQ 200000000UL //system clock frequency
#define SCLK_FREQ 100000UL //desired i2c sclk frequency
#define D 5UL //Delay val d for divide-down (see Table 20-1, t.ref manual)
#define I2C_TIMEOUT_MS 5U //amount of time slave device has to ack a byte before transaction is considered failed in ms

#define I2C_INT_XRDY_FLG (1U << 4U) //XRDY flag in I2CxSTR register
#define I2C_INT_RRDY_FLG (1U << 3U) //RRDY flag in I2CxSTR register
#define RX_MODE (I2caRegs.I2CMDR.bit.TRX == 0) //true if I2CA module is in receive mode
#define TX_MODE (I2caRegs.I2CMDR.bit.TRX == 1) //true if I2CA module is in transmit mode

extern const Semaphore_Handle i2c_tx_sem; //Semaphore used for synchronization of I2C transmitting events
extern const Semaphore_Handle i2c_rx_sem; //Semaphore used for synchronization of I2C receiving events
extern const Swi_Handle i2c_tx_swi_hdl; //SWI handle associated with SWI used for servicing I2C transmitting events
extern const Swi_Handle i2c_rx_swi_hdl; //SWI handle associated with SWI used for servicing I2C receiving events

static volatile bool nack_detected = false; //global variable set by i2c_handler_ISR if a slave NACK is detected
static volatile uint16_t data_tx_length = 0;
static volatile Uint8 *tx_buffer;
static volatile uint16_t data_rx_length = 0;
static volatile Uint8 *rx_buffer;

/***********************************************************************************
*
* i2c_init
*
* Initializes I2CA module with a SCLK rate of 400000Hz (high speed mode),
* default state is master-transmitter mode.
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
void i2c_init();

/***********************************************************************************
*
* i2c_master_tx
*
* Send a buffer of commands over I2C to a slave devices. Performs a write
* transaction of data_length bytes + address|W byte. Fails if slave NACKs any
* of the sent bytes.
*
*
* arguments:
*           dev_addr - address of i2c slave device
*           data - a pointer to the first element of the command buffer
*           data_length - the length of command bytes to be sent
*
* return:   true if successful communication, false if transmission failed
*
* Author:    Myles Parfeniuk
* Date:      10/09/2023
* Modified:  12/02/2023
************************************************************************************/
bool i2c_master_tx(Uint8 dev_addr, const Uint8 *data, uint16_t data_length);

/***********************************************************************************
*
* i2c_master_rx
*
* Receive bytes from slave device. Performs a read transaction of data_length + address|R
* byte.
*
*
* arguments:
*           dev_addr - address of i2c slave device
*           data - a pointer to the first element of the data buffer to store received bytes
*           data_length - the length of data bytes to be received
*
* return:   true if successful communication, false if transmission failed
*
* Author:    Myles Parfeniuk
* Date:      10/09/2023
* Modified:  12/02/2023
************************************************************************************/
bool i2c_master_rx(Uint8 dev_addr, Uint8 *data, uint16_t data_length);


/***********************************************************************************
*
* i2c_nack_check
*
* Module-scoped function that checks for slave NACK.
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
static bool i2c_nack_check();

void i2c_tx_handler_SWI(void);
void i2c_rx_handler_SWI(void);

/***********************************************************************************
*
* i2c_handler_ISR
*
* I2C interrupt handler service routine. All I2C interrupts are muxed through
* arbiter into single interrupt request to CPU (see figure 20-17 of t.ref manual).
*
* This handler is used to take action when the XRDY (transmit ready), RRDY (receive ready),
* or NACK (not acknowledge) interrupt flags are asserted. It posts a semaphore leading
* to the continuation of the execution of either the i2c_master_tx or i2c_master_rx function accordingly.
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
void i2c_handler_ISR(void);



#endif /* _28379D_I2C_H_ */
