/*
 * HM01B0_CAPTURE.h
 *
 *  Created on: Nov 25, 2018
 *      Author: Ali Najafi
 */

#ifndef HM01B0_CAPTURE_H_
#define HM01B0_CAPTURE_H_
//
///*test add*/
//#include <nrfx.h>
//#include <nrfx_spis.h>
//
////#if NRFX_CHECK(NRFX_SPIS_ENABLED)
////
////#if !(NRFX_CHECK(NRFX_SPIS0_ENABLED) || NRFX_CHECK(NRFX_SPIS1_ENABLED) || \
////      NRFX_CHECK(NRFX_SPIS2_ENABLED))
////#error "No enabled SPIS instances. Check <nrfx_config.h>."
////#endif
//
//
////#include "prs/nrfx_prs.h"
//
//#define NRFX_LOG_MODULE SPIS
//#include <nrfx_log.h>
///*test add finish*/



#include "HM01B0_GPIO.h"
#include "HM01B0_FUNC.h"
#include "HM01B0_CLK.h"
#include "HM01B0_SPI.h"
#include "HM01B0_COMPRESS.h"
#include "HM01B0_BLE_DEFINES.h"

//#include "HM01B0_GPIO.h"


//static uint32_t ble_bytes_sent_counter = 0;
//uint32_t      compressed_size;
uint32_t ble_bytes_sent_counter = 0;

void cam_power_up(void);

void hm01b0_init(void);

void hm_peripheral_uninit(void);

void hm_peripheral_init(void);
void hm_peripheral_connected_init(void);

void hm_single_capture(void);


void hm_single_capture_spi(void);

void hm_single_capture_spi_832(void);

void hm_single_capture_spi_832_stream(void);

void hm_single_capture_spi_832_compressed(void);


#endif /* HM01B0_CAPTURE_H_ */