/*
 * HM01B0_SPI.h
 *
 *  Created on: Nov 20, 2018
 *      Author: Ali Najafi
 */

#ifndef HM01B0_SPI_H_
#define HM01B0_SPI_H_

/*  Standard C Included Files */
#include <stdbool.h>
//#include <stdio.h>
//#include <string.h>


#include "sdk_config.h"
#include "nrf_drv_spis.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "boards.h"
#include "app_error.h"
#include <string.h>

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"




/*******************************************************************************
 * Added for SPIS initialization minimization
 ******************************************************************************/
//#include <nrfx.h>
//#include <nrfx_spis.h>
//#include "prs/nrfx_prs.h"
//#include <nrfx_log.h>
//
///**@brief States of the SPI transaction state machine. */
//typedef enum
//{
//    SPIS_STATE_INIT,                                 /**< Initialization state. In this state the module waits for a call to @ref spi_slave_buffers_set. */
//    SPIS_BUFFER_RESOURCE_REQUESTED,                  /**< State where the configuration of the memory buffers, which are to be used in SPI transaction, has started. */
//    SPIS_BUFFER_RESOURCE_CONFIGURED,                 /**< State where the configuration of the memory buffers, which are to be used in SPI transaction, has completed. */
//    SPIS_XFER_COMPLETED                              /**< State where SPI transaction has been completed. */
//} nrfx_spis_state_t;
//
///**@brief SPIS control block - driver instance local data. */
//typedef struct
//{
//    volatile uint32_t          tx_buffer_size;  //!< SPI slave TX buffer size in bytes.
//    volatile uint32_t          rx_buffer_size;  //!< SPI slave RX buffer size in bytes.
//    nrfx_spis_event_handler_t  handler;         //!< SPI event handler.
//    volatile const uint8_t *   tx_buffer;       //!< SPI slave TX buffer.
//    volatile uint8_t *         rx_buffer;       //!< SPI slave RX buffer.
//    nrfx_drv_state_t           state;           //!< driver initialization state.
//    volatile nrfx_spis_state_t spi_state;       //!< SPI slave state.
//    void *                     p_context;       //!< Context set on initialization.
//} spis_cb_t;
//
//static spis_cb_t m_cb[NRFX_SPIS_ENABLED_COUNT];


#include "HM01B0_BLE_DEFINES.h"

//
//#define SPIS_INSTANCE 2 /**< SPIS instance index. */
static const nrf_drv_spis_t spis = NRF_DRV_SPIS_INSTANCE(SPIS_INSTANCE);/**< SPIS instance. */
//static nrf_drv_spis_t spis = NRF_DRV_SPIS_INSTANCE(SPIS_INSTANCE);/**< SPIS instance. */

//#define TEST_STRING "Nordic"
//#define TEST_STRING "N"
//static uint8_t       m_tx_buf[] = TEST_STRING;           /**< TX buffer. */
static uint8_t       m_tx_buf[1] = {0} ;                           /**< TX buffer. */
//static uint8_t       m_rx_buf[sizeof(TEST_STRING) + 1];    /**< RX buffer. */
//static const uint8_t m_length = sizeof(m_tx_buf);        /**< Transfer length. */
extern uint8_t       m_rx_buf[total_spi_buffer_size_max+200];       /**< RX buffer. 200 added for the ACC and Mag data */
extern uint16_t m_length_rx;        /**< Transfer length. */
extern uint16_t m_length_rx_done = 0;        /**< Transfer length. */
//static const uint8_t m_length_tx = 0;        /**< Transfer length. */
static uint8_t m_length_tx = 0;        /**< Transfer length. */

extern bool spis_xfer_done; /**< Flag used to indicate that SPIS instance completed the transfer. */
uint8_t accel_sr;


/*******************************************************************************
 * SPI Functions
 ******************************************************************************/
/*!
* @brief SPI settings initialization.
*/

void spis_event_handler(nrf_drv_spis_event_t event);

void spis_pin_set(void);

void spi_init(void);

nrfx_err_t nrf_drv_spis_buffers_set_832(nrfx_spis_t const * const p_instance,
                                 uint8_t           const * p_tx_buffer,
                                 size_t                    tx_buffer_length,
                                 uint8_t                 * p_rx_buffer,
                                 size_t                    rx_buffer_length);

#endif /* HM01B0_SPI_H_ */
