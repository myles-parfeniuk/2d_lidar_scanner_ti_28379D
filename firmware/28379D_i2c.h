/***********************************************************************************
 * 28379D_i2c.h
 *
 * A module containing functions allowing for basic I2C communications with
 * the I2CA peripheral of the TI tm320F828379D controller.
 *
 * Author:    Myles Parfeniuk
 * Date:      10/09/2023
 * Modified:  10/09/2023
 *********************************************************************************/
#ifndef _28379D_I2C_H_
#define _28379D_I2C_H_

//TI c2000Ware libraries
#include "driverlib.h"
#include "device.h"

#define PIN_I2C_SCLK 105 //sclk gpio
#define PIN_I2C_SDA 104 //sdat gpio


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
* Modified:  10/09/2023
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
* Modified:  10/09/2023
************************************************************************************/
bool i2c_master_tx(uint8_t dev_addr, uint8_t *data, uint16_t data_length);

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
* Modified:  10/09/2023
************************************************************************************/
bool i2c_master_rx(uint8_t dev_addr, uint8_t *data, uint16_t data_length);


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
* Modified:  10/09/2023
************************************************************************************/
static bool i2c_nack_check();


#endif /* _28379D_I2C_H_ */
