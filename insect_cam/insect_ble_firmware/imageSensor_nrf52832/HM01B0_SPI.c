/*
 * HM01B0_SPI.c
 *
 *  Created on: Nov 30, 2018
 *      Author: Ali Najafi
 */


#include "HM01B0_CAPTURE.h"
#include "HM01B0_SPI.h"

#include <nrfx_spis.h>

nrf_drv_spis_config_t spis_config = NRF_DRV_SPIS_DEFAULT_CONFIG;

/*******************************************************************************
 * Code SPI
 ******************************************************************************/

/**
 * @brief SPIS user event handler.
 *
 * @param event
 */
void spis_event_handler(nrf_drv_spis_event_t event)
{
    if (event.evt_type == NRF_DRV_SPIS_XFER_DONE)
    {
        spis_xfer_done = true;
        //NRF_LOG_INFO(" Transfer completed. Received: %s",(uint32_t)m_rx_buf);
    }
}

void spis_pin_set(void)
{
    spis_config.csn_pin               = APP_SPIS_CS_PIN;//pin 29
    spis_config.miso_pin              = APP_SPIS_MISO_PIN;//pin 28
    spis_config.mosi_pin              = APP_SPIS_MOSI_PIN;//pin 4 
    spis_config.sck_pin               = APP_SPIS_SCK_PIN;//pin 3
}


void spi_init(void)
{
    // Enable the constant latency sub power mode to minimize the time it takes
    // for the SPIS peripheral to become active after the CSN line is asserted
    // (when the CPU is in sleep mode).
//    NRF_POWER->TASKS_CONSTLAT = 1;

    

//    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
//    NRF_LOG_DEFAULT_BACKENDS_INIT();

    


    APP_ERROR_CHECK(nrf_drv_spis_init(&spis, &spis_config, spis_event_handler));

//    printf('%d',sizeof(m_tx_buf));
//    memset(m_rx_buf, 0, m_length_rx);
    spis_xfer_done = false;

    APP_ERROR_CHECK(nrf_drv_spis_buffers_set(&spis, m_tx_buf, m_length_tx, m_rx_buf, m_length_rx));
    //*((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)spis_task)) = 0x1UL;
}




/*******************************************************************************
 * Code SPI for optimization
 ******************************************************************************/
//#define NRFX_LOG_MODULE SPIS
////#include <nrfx_log.h>
//
//#define EVT_TO_STR(event)                                           \
//    (event == NRF_SPIS_EVENT_ACQUIRED ? "NRF_SPIS_EVENT_ACQUIRED" : \
//    (event == NRF_SPIS_EVENT_END      ? "NRF_SPIS_EVENT_END"      : \
//                                        "UNKNOWN ERROR"))
//
//#define SPISX_LENGTH_VALIDATE(peripheral, drv_inst_idx, rx_len, tx_len) \
//    (((drv_inst_idx) == NRFX_CONCAT_3(NRFX_, peripheral, _INST_IDX)) && \
//     NRFX_EASYDMA_LENGTH_VALIDATE(peripheral, rx_len, tx_len))
//
//#if NRFX_CHECK(NRFX_SPIS0_ENABLED)
//#define SPIS0_LENGTH_VALIDATE(...)  SPISX_LENGTH_VALIDATE(SPIS0, __VA_ARGS__)
//#else
//#define SPIS0_LENGTH_VALIDATE(...)  0
//#endif
//
//#if NRFX_CHECK(NRFX_SPIS1_ENABLED)
//#define SPIS1_LENGTH_VALIDATE(...)  SPISX_LENGTH_VALIDATE(SPIS1, __VA_ARGS__)
//#else
//#define SPIS1_LENGTH_VALIDATE(...)  0
//#endif
//
//#if NRFX_CHECK(NRFX_SPIS2_ENABLED)
//#define SPIS2_LENGTH_VALIDATE(...)  SPISX_LENGTH_VALIDATE(SPIS2, __VA_ARGS__)
//#else
//#define SPIS2_LENGTH_VALIDATE(...)  0
//#endif
//
//#define SPIS_LENGTH_VALIDATE(drv_inst_idx, rx_len, tx_len)  \
//    (SPIS0_LENGTH_VALIDATE(drv_inst_idx, rx_len, tx_len) || \
//     SPIS1_LENGTH_VALIDATE(drv_inst_idx, rx_len, tx_len) || \
//     SPIS2_LENGTH_VALIDATE(drv_inst_idx, rx_len, tx_len))
//
//static void spis_state_entry_action_execute(NRF_SPIS_Type * p_spis,
//                                            spis_cb_t     * p_cb)
//{
//    nrfx_spis_evt_t event;
//
//    switch (p_cb->spi_state)
//    {
//        case SPIS_BUFFER_RESOURCE_REQUESTED:
//            nrf_spis_task_trigger(p_spis, NRF_SPIS_TASK_ACQUIRE);
//            break;
//
//        case SPIS_BUFFER_RESOURCE_CONFIGURED:
//            event.evt_type  = NRFX_SPIS_BUFFERS_SET_DONE;
//            event.rx_amount = 0;
//            event.tx_amount = 0;
//
//            NRFX_ASSERT(p_cb->handler != NULL);
//            p_cb->handler(&event, p_cb->p_context);
//            break;
//
//        case SPIS_XFER_COMPLETED:
//            event.evt_type  = NRFX_SPIS_XFER_DONE;
//            event.rx_amount = nrf_spis_rx_amount_get(p_spis);
//            event.tx_amount = nrf_spis_tx_amount_get(p_spis);
//            NRFX_LOG_INFO("Transfer rx_len:%d.", event.rx_amount);
//            NRFX_LOG_DEBUG("Rx data:");
//            NRFX_LOG_HEXDUMP_DEBUG((uint8_t const *)p_cb->rx_buffer,
//                                   event.rx_amount * sizeof(p_cb->rx_buffer[0]));
//            NRFX_ASSERT(p_cb->handler != NULL);
//            p_cb->handler(&event, p_cb->p_context);
//            break;
//
//        default:
//            // No implementation required.
//            break;
//    }
//}
//
//
//
//static void spis_state_change(NRF_SPIS_Type   * p_spis,
//                              spis_cb_t       * p_cb,
//                              nrfx_spis_state_t new_state)
//{
//    p_cb->spi_state = new_state;
//    spis_state_entry_action_execute(p_spis, p_cb);
//}
//
//nrfx_err_t nrf_drv_spis_buffers_set_832(nrfx_spis_t const * const p_instance,
//                                 uint8_t           const * p_tx_buffer,
//                                 size_t                    tx_buffer_length,
//                                 uint8_t                 * p_rx_buffer,
//                                 size_t                    rx_buffer_length)
//{
//    NRFX_ASSERT(p_tx_buffer != NULL || tx_buffer_length == 0);
//    NRFX_ASSERT(p_rx_buffer != NULL || rx_buffer_length == 0);
//
//    spis_cb_t * p_cb = &m_cb[p_instance->drv_inst_idx];
//    nrfx_err_t err_code;
//
//    if (!SPIS_LENGTH_VALIDATE(p_instance->drv_inst_idx,
//                              rx_buffer_length,
//                              tx_buffer_length))
//    {
//        return NRFX_ERROR_INVALID_LENGTH;
//    }
//
//    // EasyDMA requires that transfer buffers are placed in Data RAM region;
//    // signal error if they are not.
//    if ((p_tx_buffer != NULL && !nrfx_is_in_ram(p_tx_buffer)) ||
//        (p_rx_buffer != NULL && !nrfx_is_in_ram(p_rx_buffer)))
//    {
//        err_code = NRFX_ERROR_INVALID_ADDR;
//        NRFX_LOG_WARNING("Function: %s, error code: %s.",
//                         __func__,
//                         NRFX_LOG_ERROR_STRING_GET(err_code));
//        return err_code;
//    }
//
//    switch (p_cb->spi_state)
//    {
//        case SPIS_STATE_INIT:
//        case SPIS_XFER_COMPLETED:
//        case SPIS_BUFFER_RESOURCE_CONFIGURED:
//            p_cb->tx_buffer      = p_tx_buffer;
//            p_cb->rx_buffer      = p_rx_buffer;
//            p_cb->tx_buffer_size = tx_buffer_length;
//            p_cb->rx_buffer_size = rx_buffer_length;
//            err_code             = NRFX_SUCCESS;
//
//            spis_state_change(p_instance->p_reg, p_cb, SPIS_BUFFER_RESOURCE_REQUESTED);
//            break;
//
//        case SPIS_BUFFER_RESOURCE_REQUESTED:
//            err_code = NRFX_ERROR_INVALID_STATE;
//            break;
//
//        default:
//            // @note: execution of this code path would imply internal error in the design.
//            err_code = NRFX_ERROR_INTERNAL;
//            break;
//    }
//
//    NRFX_LOG_INFO("Function: %s, error code: %s.", __func__, NRFX_LOG_ERROR_STRING_GET(err_code));
//    return err_code;
//}

