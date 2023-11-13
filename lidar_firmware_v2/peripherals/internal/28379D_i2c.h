/***********************************************************************************
 * 28379D_i2c.h
 *
 * A module containing functions allowing for basic I2C communications with
 * the I2CA peripheral of the TI tm320F828379D controller.
 *
 * Author:    Myles Parfeniuk
 * Date:      10/09/2023
 * Modified:  10/20/2023
 *********************************************************************************/
#ifndef _28379D_I2C_H_
#define _28379D_I2C_H_

//C standard library includes
#include <ctype.h>

//TI includes
#include <Headers/F2837xD_device.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

#define SYS_CLK_FREQ 200000000UL //system clock frequency
#define SCLK_FREQ 100000UL //desired i2c sclk frequency
#define D 5UL //Delay val d for divide-down (see Table 20-1, t.ref manual)
#define I2C_TIMEOUT_MS 5U //amount of time slave device has to ack a byte before transaction is considered failed in ms
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
* Modified:  10/20/2023
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
* Modified:  10/20/2023
************************************************************************************/
bool i2c_master_tx(char dev_addr, const char *data, uint16_t data_length);

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
* Modified:  10/20/2023
************************************************************************************/
bool i2c_master_rx(char dev_addr, char *data, uint16_t data_length);


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
* Modified:  10/20/2023
************************************************************************************/
static bool i2c_nack_check();


#endif /* _28379D_I2C_H_ */
