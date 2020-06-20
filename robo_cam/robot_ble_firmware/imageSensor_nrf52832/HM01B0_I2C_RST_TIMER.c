/*
 * HM01B0_I2C_RST_TIMER.c
 *
 *  Created on: May 17, 2019
 *      Author: Ali Najafi
 */

#include "HM01B0_LVLD_TIMER.h"



/**
 * @brief Handler for timer events.
 */
void timer_i2c_rst_event_handler(nrf_timer_event_t event_type, void* p_context)
{
    NRF_LOG_WARNING("System reset");
    NVIC_SystemReset();
}

void i2c_rst_timer_init(void)
{
    uint32_t err_code = NRF_SUCCESS;
    //Configure TIMER_LED for generating simple light effect - leds on board will invert his state one after the other.
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    err_code = nrf_drv_timer_init(&TIMER_I2C_RST, &timer_cfg, timer_i2c_rst_event_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_timer_extended_compare(
         &TIMER_I2C_RST, NRF_TIMER_CC_CHANNEL2, i2c_rst_timer_val, NRF_TIMER_SHORT_COMPARE2_CLEAR_MASK, true);

    //nrf_drv_timer_enable(&TIMER_LVLD);
}

/** @} */
