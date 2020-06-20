/*
 * MC6470_FUNC.h
 *
 *  Created on: Apr 26, 2019
 *      Author: Ali Najafi
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

#include "MC6470Regs.h"

extern uint8_t acc_xout_l;
extern uint8_t acc_xout_h;

extern uint8_t acc_yout_l;
extern uint8_t acc_yout_h;

extern uint8_t acc_zout_l;
extern uint8_t acc_zout_h;

uint8_t       m_rx_buf[total_spi_buffer_size_max+200];
uint8_t acc_rec_counter;
uint16_t m_length_rx;


extern uint8_t accel_sr;

void acc_power_up(void);

void acc_power_down(void);

void mc6470_mag_init(void);

void mc6470_acc_init(void);

void mc6470_acc_read(void);


#endif