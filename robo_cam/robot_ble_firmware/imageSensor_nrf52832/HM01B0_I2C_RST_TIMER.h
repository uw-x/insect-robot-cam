/*
 * HM01B0_I2C_RST_TIMER.h
 *
 *  Created on: May 17, 2019
 *      Author: Ali Najafi
 */

#ifndef HM01B0_I2C_RST_TIMER_H_
#define HM01B0_I2C_RST_TIMER_H_

#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_drv_timer.h"
#include "app_error.h"
#include "HM01B0_BLE_DEFINES.h"
#include "HM01B0_GPIO.h"
#include "HM01B0_CAPTURE.h"

const nrf_drv_timer_t TIMER_I2C_RST = NRF_DRV_TIMER_INSTANCE(2);

extern uint32_t i2c_rst_timer_val = I2C_RST_TIMER_VALUE;

void timer_i2c_rst_event_handler(nrf_timer_event_t event_type, void* p_context);

void i2c_rst_timer_init(void);


#endif /* HM01B0_I2C_RST_TIMER_H_ */