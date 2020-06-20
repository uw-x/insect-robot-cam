/*
 * HM01B0_I2c.h
 *
 *  Created on: NOV 25, 2018
 *      Author: Ali Najafi
 */

#ifndef HM01B0_I2C_H_
#define HM01B0_I2C_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

//#include "nrf.h"
//#include "nrf_gpiote.h"
//#include "nrf_gpio.h"
//#include "nrf_drv_ppi.h"
//#include "nrf_drv_timer.h"
//#include "nrf_drv_gpiote.h"

#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_delay.h"
#include "HM01B0_BLE_DEFINES.h"
#include "HM01B0_I2C_RST_TIMER.h"


/*Added by Ali*/
//#include "HM01B0_I2C.h"
//#include "HM01B0_CLK.h"
#include "HM01B0Regs.h"

#define SLAVE_ADDR 0x24U //address for camera
//#define SLAVE_ADDR 0x0CU //address for accel
//#define I2C_MASTER_SLAVE_ADDR 0x7EU
#define I2C_DATA_LENGTH 32U

/* TWI instance ID. */
#define TWI_INSTANCE_ID     0

/* Indicates if operation on TWI has ended. */
//static volatile bool m_xfer_done = false;

/* TWI instance. */
//static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

/* Buffer for samples read from temperature sensor. */
//static uint8_t m_sample;

/**
 * @brief Function for setting active mode on MMA7660 accelerometer.
 */

/**
 * @brief Function for handling data from temperature sensor.
 *
 * @param[in] temp          Temperature in Celsius degrees read from sensor.
 */
__STATIC_INLINE void data_handler(uint8_t temp);

/**
 * @brief TWI events handler.
 */
void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context);
void twi_uninit(void);
void twi_init (void);
/**
 * @brief Function for reading data from temperature sensor.
 */
static void read_sensor_data();

/**
 * @brief Function for reading data from image sensor.
 */
uint8_t hm_i2c_read(uint16_t addr);

bool hm_i2c_write_check(uint16_t addr, uint8_t reg);

void hm_i2c_write(uint16_t addr, uint8_t reg);

uint8_t hm_i2c_read_8b(uint8_t addr, uint8_t slave_addr);

bool hm_i2c_write_check_8b(uint8_t addr, uint8_t reg, uint8_t slave_addr);
void hm_i2c_write_8b(uint8_t addr, uint8_t reg, uint8_t slave_addr);

#endif /* HM01B0_I2C_H_ */
