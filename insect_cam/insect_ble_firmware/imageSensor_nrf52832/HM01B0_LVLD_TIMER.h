/*
 * HM01B0_LVLD_TIMER.h
 *
 *  Created on: April 7, 2019
 *      Author: Ali
 */

#ifndef HM01B0_LVLD_TIMER_H_
#define HM01B0_LVLD_TIMER_H_

#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_drv_timer.h"
#include "app_error.h"
#include "HM01B0_BLE_DEFINES.h"
#include "HM01B0_GPIO.h"
#include "HM01B0_CAPTURE.h"

uint8_t acc_rec_counter;

const nrf_drv_timer_t TIMER_LVLD = NRF_DRV_TIMER_INSTANCE(4);

extern uint32_t lvld_timer_val = LVLD_TIMER_VALUE;

void timer_lvld_event_handler(nrf_timer_event_t event_type, void* p_context);

void lvld_timer_init(void);


#endif /* HM01B0_LVLD_TIMER_H_ */