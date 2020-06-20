/**
 * Copyright (c) 2015 - 2018, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 * @defgroup pwm_example_main main.c
 * @{
 * @ingroup pwm_example
 *
 * @brief  PWM Example Application main file.
 *
 * This file contains the source code for a sample application using PWM.
 *
 *
 */

#include "HM01B0_PWM_BOOST.h"


/**
 * @brief Handler for timer events.
 */
void timer_pwm_boost_event_handler(nrf_timer_event_t event_type, void* p_context)
{
    nrf_gpio_pin_set(BOOST_CLK_PIN);
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");

//    asm("nop");
//    asm("nop");
//    asm("nop");
//    asm("nop");

    nrf_gpio_pin_clear(BOOST_CLK_PIN);
}

void timer_pwm_boost_ramp(uint32_t final_timer_boost_value, uint8_t delay){
    uint8_t i=0;

    while(TIMER_VALUES_PWM_BOOST[TIMER_BOOST_ARRAY_SIZE-i-1]>=final_timer_boost_value){
        nrf_drv_timer_disable(&PWM_BOOST_TIMER);
        nrf_drv_timer_extended_compare(
         &PWM_BOOST_TIMER, NRF_TIMER_CC_CHANNEL2, TIMER_VALUES_PWM_BOOST[TIMER_BOOST_ARRAY_SIZE-i-1], NRF_TIMER_SHORT_COMPARE2_CLEAR_MASK, true);
         nrf_drv_timer_enable(&PWM_BOOST_TIMER);
         nrf_delay_ms(delay);
         i++;
         }
}

void timer_pwm_boost_init(void)
{
    uint32_t err_code = NRF_SUCCESS;
    //Configure TIMER_LED for generating simple light effect - leds on board will invert his state one after the other.
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    err_code = nrf_drv_timer_init(&PWM_BOOST_TIMER, &timer_cfg, timer_pwm_boost_event_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_timer_extended_compare(
         &PWM_BOOST_TIMER, NRF_TIMER_CC_CHANNEL4, TIMER_VALUES_PWM_BOOST[TIMER_BOOST_ARRAY_SIZE-1], NRF_TIMER_SHORT_COMPARE4_CLEAR_MASK, true);

    nrf_gpio_cfg_output(BOOST_CLK_PIN);
    nrf_gpio_pin_clear(BOOST_CLK_PIN);
}

void pwm_boost_activate(void)
{

    timer_pwm_boost_init();
//    printf("test: %d \n", TIMER_VALUES_PWM_BOOST[TIMER_BOOST_ARRAY_SIZE-1]);
    nrf_drv_timer_enable(&PWM_BOOST_TIMER);

}


/** @} */
