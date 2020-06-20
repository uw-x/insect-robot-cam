/*
 * motor.h
 *
 *  Created on: Nov 16, 2019
 *      Author: Vikram 
 */

 #ifndef MC6470_FUNC_H
#define MC6470_FUNC_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_delay.h"
#include "HM01B0_BLE_DEFINES.h"

#define LEFT 0
#define RIGHT 1

void motor_start(uint8_t side);

void motor_stop(uint8_t side);

void rt8092_init(void);

void step_forward(void);

void turn_right(void);

void turn_left(void);

void stop_robot(void);

#endif