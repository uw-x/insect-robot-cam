/*
 * HM01B0_CLK.h
 *
 *  Created on: Nov 25, 2018
 *      Author: Ali Najafi
 */

#ifndef HM01B0_CLK_H_
#define HM01B0_CLK_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "nrf.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_gpiote.h"

#include "boards.h"
#include "app_error.h"

#include "HM01B0_BLE_DEFINES.h"

extern    uint32_t* timer0_prescaler_address = 0x40009510;//address for timer1 prescaler
extern    uint32_t timer0_prescaler1 = 0x00000001;
extern    uint32_t timer0_prescaler0 = 0x00000000;


static nrf_drv_timer_t CAM_TIMER = NRF_DRV_TIMER_INSTANCE(1);

static void timer_dummy_handler(nrf_timer_event_t event_type, void * p_context){}

void set_timer0_prescaler(uint32_t timer0_prescaler);

void hm_clk_out(void);

#endif /* HM01B0_CLK_H_ */