//in-house libraries
#include "28379D_i2c.h"

void i2c_init()
{
    //gpio initialization SCLK - IO_105, SDA - IO_104
    GPIO_setDirectionMode(PIN_I2C_SDA, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(PIN_I2C_SDA, GPIO_PIN_TYPE_PULLUP);
    GPIO_setMasterCore(PIN_I2C_SDA, GPIO_CORE_CPU1);
    GPIO_setQualificationMode(PIN_I2C_SDA, GPIO_QUAL_ASYNC);

    GPIO_setDirectionMode(PIN_I2C_SCLK, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(PIN_I2C_SCLK, GPIO_PIN_TYPE_PULLUP);
    GPIO_setMasterCore(PIN_I2C_SCLK, GPIO_CORE_CPU1);
    GPIO_setQualificationMode(PIN_I2C_SCLK, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(GPIO_104_SDAA);
    GPIO_setPinConfig(GPIO_105_SCLA);

    I2C_disableModule(I2CA_BASE); //disable i2c module A for setup
    I2C_initMaster(I2CA_BASE, SysCtl_getClock(DEVICE_OSCSRC_FREQ), 400000U, I2C_DUTYCYCLE_50); //init master for 100KHz sclk, 55% clock duty cycle
    I2C_setConfig(I2CA_BASE, I2C_MASTER_SEND_MODE);
    I2C_disableLoopback(I2CA_BASE);
    I2C_setBitCount(I2CA_BASE, I2C_BITCOUNT_8);
    I2C_setAddressMode(I2CA_BASE, I2C_ADDR_MODE_7BITS);
    I2C_disableFIFO(I2CA_BASE);
    I2C_enableModule(I2CA_BASE);

}


bool i2c_master_tx(uint8_t dev_addr, uint8_t *data, uint16_t data_length)
{

    uint16_t i = 0;


    I2C_setConfig(I2CA_BASE, I2C_MASTER_SEND_MODE);
    I2C_setDataCount(I2CA_BASE, data_length);
    I2C_setSlaveAddress(I2CA_BASE, dev_addr); //set target address for transaction

    I2C_sendStartCondition(I2CA_BASE);


    for(i = 0; i < data_length; i++)
    {

        while (!(I2C_getInterruptStatus(I2CA_BASE) & I2C_INT_TX_DATA_RDY)) {
            // Wait for the transmit ready flag

            if(i2c_nack_check())
            {
                return false;
            }
        }


        I2C_putData(I2CA_BASE, data[i]);

    }



    I2C_sendStopCondition(I2CA_BASE);

    return true;


}

bool i2c_master_rx(uint8_t dev_addr, uint8_t *data, uint16_t data_length)
{
    uint16_t i = 0;

    i2c_init();
    I2C_setConfig(I2CA_BASE, I2C_MASTER_RECEIVE_MODE);
    I2C_setDataCount(I2CA_BASE, data_length);
    I2C_setSlaveAddress(I2CA_BASE, dev_addr); // Set target address for transaction

    I2C_sendStartCondition(I2CA_BASE);

    for (i = 0; i < data_length; i++)
    {
        // Wait until data is received
        while (!(I2C_getInterruptStatus(I2CA_BASE) & I2C_INT_RX_DATA_RDY))
        {
            // Check for NACK
            if (i2c_nack_check())
            {
                return false;
            }

        }

        // If it's the last byte, send NACK before stopping
        if (i == (data_length - 2))
        {
            I2C_sendNACK(I2CA_BASE);
        }

        // Read the received data
        data[i] = (uint8_t)I2C_getData(I2CA_BASE);

    }

    I2C_sendStopCondition(I2CA_BASE);

    return true;
}

bool i2c_nack_check()
{
    if(I2C_getStatus(I2CA_BASE) & I2C_STS_NO_ACK)
    {


               I2C_clearStatus(I2CA_BASE, (I2C_STS_NO_ACK | I2C_STS_TX_DATA_RDY));
               I2C_sendStopCondition(I2CA_BASE);

               return true;
    }

    return false;
}
