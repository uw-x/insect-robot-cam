/*
 * MC6470_FUNC.c
 *
 *  Created on: Oct 26, 2018
 *      Author: Ali Najafi
 */



/*Added by Ali*/
#include "HM01B0_I2C.h"
//#include "HM01B0_CLK.h"
//#include "HM01B0Regs.h"




/* Indicates if operation on TWI has ended. */
static volatile bool m_xfer_done = false;
uint8_t i2c_rst_counter = 0;
bool i2c_while_stuck = false;

/* TWI instance. */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

/* Buffer for samples read from temperature sensor. */
static uint8_t m_sample;


/**
 * @brief Function for handling data from temperature sensor.
 *
 * @param[in] temp          Temperature in Celsius degrees read from sensor.
 */
__STATIC_INLINE void data_handler(uint8_t temp)
{
//    #if (FINAL_CODE == 0)
//     NRF_LOG_INFO("Temperature: %d Celsius degrees.", temp);
//    #endif
    
}

/**
 * @brief TWI events handler.
 */
void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
    switch (p_event->type)
    {
        case NRF_DRV_TWI_EVT_DONE:
            if (p_event->xfer_desc.type == NRF_DRV_TWI_XFER_RX)
            {
                data_handler(m_sample);
            }
            m_xfer_done = true;
            break;
        default:
            break;
    }
}


void twi_uninit(void)
{
    nrfx_twim_uninit(&m_twi);
}

/**
 * @brief UART initialization.
 */
void twi_init (void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_SLAVE_config = {
       .scl                = 26,
       .sda                = 27,
//       .scl                = ARDUINO_SCL_PIN,
//       .sda                = ARDUINO_SDA_PIN,
       .frequency          = NRF_DRV_TWI_FREQ_100K,
//       .frequency          = NRF_TWI_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_SLAVE_config, twi_handler, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
    
    i2c_rst_timer_init();

    nrf_delay_ms(100);
}

/**
 * @brief Function for reading data from temperature sensor.
 */
static void read_sensor_data()
{
    m_xfer_done = false;

    /* Read 1 byte from the specified address - skip 3 bits dedicated for fractional part of temperature. */
    ret_code_t err_code = nrf_drv_twi_rx(&m_twi, SLAVE_ADDR, &m_sample, sizeof(m_sample));
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function for reading data from image sensor.
 */
uint8_t hm_i2c_read(uint16_t addr)
{
    i2c_while_stuck = false;
    uint8_t data_out;
    uint8_t reg_addr[2];

    reg_addr[0] = (addr >> 8) & 0xFF;    // addr MSB
    reg_addr[1] = addr & 0xFF;           // addr LSB

    ret_code_t err_code;

    /* Writing to SLAVE_REG_CONF "0" set temperature sensor in NORMAL mode. */
    err_code = nrf_drv_twi_tx(&m_twi, SLAVE_ADDR, reg_addr, sizeof(reg_addr), false);
    APP_ERROR_CHECK(err_code);

    #if(I2C_RST_ENABLE == 0)
    while (m_xfer_done == false);
    #else
    while (m_xfer_done == false){
        if(!i2c_while_stuck){
            i2c_while_stuck = true;
            nrf_drv_timer_enable(&TIMER_I2C_RST);
        }
    }
    nrf_drv_timer_disable(&TIMER_I2C_RST);
    #endif

    m_xfer_done = false;

    /* Read 1 byte from the specified address - skip 3 bits dedicated for fractional part of temperature. */
    err_code = nrf_drv_twi_rx(&m_twi, SLAVE_ADDR, &data_out, sizeof(data_out));
//    APP_ERROR_CHECK(err_code);

    #if(I2C_RST_ENABLE == 0)
    while (m_xfer_done == false);
    #else
    while (m_xfer_done == false){
        if(!i2c_while_stuck){
            i2c_while_stuck = true;
            nrf_drv_timer_enable(&TIMER_I2C_RST);
        }
    }
    nrf_drv_timer_disable(&TIMER_I2C_RST);
    #endif

    m_xfer_done = false;

    return data_out;
}

bool hm_i2c_write_check(uint16_t addr, uint8_t reg){
    uint8_t rdData = hm_i2c_read(addr);
    if( rdData == reg){
        return true;
    } else {
        return false;
    }

}

void hm_i2c_write(uint16_t addr, uint8_t reg)
{
    uint8_t reg_addr[3];
    i2c_while_stuck = false;

    reg_addr[0] = (addr >> 8) & 0xFF;    // addr MSB
    reg_addr[1] = addr & 0xFF;           // addr LSB
    reg_addr[2] = reg;

    ret_code_t err_code;

    /* Writing to SLAVE_REG_CONF "0" set temperature sensor in NORMAL mode. */
    err_code = nrf_drv_twi_tx(&m_twi, SLAVE_ADDR, reg_addr, sizeof(reg_addr), false);
    APP_ERROR_CHECK(err_code);

    #if(I2C_RST_ENABLE == 0)
    while (m_xfer_done == false);
    #else
    while (m_xfer_done == false){
        if(!i2c_while_stuck){
            i2c_while_stuck = true;
            nrf_drv_timer_enable(&TIMER_I2C_RST);
        }
    }
    nrf_drv_timer_disable(&TIMER_I2C_RST);
    #endif

    m_xfer_done = false;

//    if (hm_i2c_write_check(addr, reg) == false){
//        printf("WR_F!\n");
//        hm_i2c_write (addr, reg);
//    }

    if (hm_i2c_write_check(addr, reg) == false){
        if(i2c_rst_counter>I2C_RST_CNT_MAX){
            NRF_LOG_WARNING("System reset");
            NVIC_SystemReset();
        }
        i2c_rst_counter++;
        hm_i2c_write(addr, reg);
    }
    i2c_rst_counter = 0;
}

/**
 * @brief Function for reading data from image sensor.
 */
uint8_t hm_i2c_read_8b(uint8_t addr, uint8_t slave_addr)
{
    i2c_while_stuck = false;
    uint8_t data_out;
    uint8_t reg_addr[1];

    reg_addr[0] = addr & 0xFF;           // addr LSB

    ret_code_t err_code;

    /* Writing to SLAVE_REG_CONF "0" set temperature sensor in NORMAL mode. */
    err_code = nrf_drv_twi_tx(&m_twi, slave_addr, reg_addr, sizeof(reg_addr), false);
    APP_ERROR_CHECK(err_code);

    #if(I2C_RST_ENABLE == 0)
    while (m_xfer_done == false);
    #else
    while (m_xfer_done == false){
        if(!i2c_while_stuck){
            i2c_while_stuck = true;
            nrf_drv_timer_enable(&TIMER_I2C_RST);
        }
    }
    nrf_drv_timer_disable(&TIMER_I2C_RST);
    #endif

    m_xfer_done = false;

    /* Read 1 byte from the specified address - skip 3 bits dedicated for fractional part of temperature. */
    err_code = nrf_drv_twi_rx(&m_twi, slave_addr, &data_out, sizeof(data_out));
    APP_ERROR_CHECK(err_code);

    #if(I2C_RST_ENABLE == 0)
    while (m_xfer_done == false);
    #else
    while (m_xfer_done == false){
        if(!i2c_while_stuck){
            i2c_while_stuck = true;
            nrf_drv_timer_enable(&TIMER_I2C_RST);
        }
    }
    nrf_drv_timer_disable(&TIMER_I2C_RST);
    #endif

    m_xfer_done = false;

    return data_out;
}

bool hm_i2c_write_check_8b(uint8_t addr, uint8_t reg, uint8_t slave_addr){
    uint8_t rdData = hm_i2c_read_8b(addr, slave_addr);
    if( rdData == reg){
        return true;
    } else {
        return false;
    }

}

void hm_i2c_write_8b(uint8_t addr, uint8_t reg, uint8_t slave_addr)
{
    i2c_while_stuck = false;
    uint8_t reg_addr[2];

    reg_addr[0] = addr & 0xFF;           // addr LSB
    reg_addr[1] = reg;

    ret_code_t err_code;

    /* Writing to SLAVE_REG_CONF "0" set temperature sensor in NORMAL mode. */
    err_code = nrf_drv_twi_tx(&m_twi, slave_addr, reg_addr, sizeof(reg_addr), false);
    APP_ERROR_CHECK(err_code);

    #if(I2C_RST_ENABLE == 0)
    while (m_xfer_done == false);
    #else
    while (m_xfer_done == false){
        if(!i2c_while_stuck){
            i2c_while_stuck = true;
            nrf_drv_timer_enable(&TIMER_I2C_RST);
        }
    }
    nrf_drv_timer_disable(&TIMER_I2C_RST);
    #endif
    

    m_xfer_done = false;

//    if (hm_i2c_write_check_8b(addr, reg, slave_addr) == false){
//        printf("WR_F!\n");
//        hm_i2c_write_8b (addr, reg, slave_addr);
//    }

    if (hm_i2c_write_check_8b(addr, reg, slave_addr) == false){
        if(i2c_rst_counter>I2C_RST_CNT_MAX){
            NRF_LOG_WARNING("System reset");
            NVIC_SystemReset();
        }
        i2c_rst_counter++;
        hm_i2c_write(addr, reg);
    }
    i2c_rst_counter = 0;
}



