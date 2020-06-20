/*
 * HM01B0_LVLD_TIMER.h
 *
 *  Created on: April 25, 2019
 *      Author: Ali Najafi
 */

#include "HM01B0_LVLD_TIMER.h"



/**
 * @brief Handler for timer events.
 */
void timer_lvld_event_handler(nrf_timer_event_t event_type, void* p_context)
{
    if (line_count<LINE_NUM){
    // here we need to activate SPI CS; enable the lvld_timer; and activate the line_vld interrupt; increase the counter of lines
        lvld_timer_pause(&TIMER_LVLD);
        gpio_p_reg->OUTSET = CAM_SPI_PIN_MASK;
        nrfx_spis_buffers_set_back(&spis, m_tx_buf, m_length_tx, m_rx_buf + line_count*m_length_rx, m_length_rx);
        nrf_drv_gpiote_in_event_enable(LINE_VLD, true);
        } 
    else{        
        nrf_drv_gpiote_in_event_disable(LINE_VLD);
        nrf_drv_gpiote_in_event_disable(FRAME_VLD);
        lvld_timer_pause(&TIMER_LVLD);
        gpio_p_reg->OUTSET = CAM_SPI_PIN_MASK;

        #if (FRAME_VLD_INT == 1)
      
          #if defined(BOARD_PCA10056)
          m_length_rx_done = m_length_rx;
          #endif

          #if defined(BOARD_PCA10040)
          m_length_rx_done = total_image_size;
          #endif
          ble_bytes_sent_counter = 0;

        /*SPI registers initilization*/
          #if (MEM_INIT == 1)
            memset(m_rx_buf, MEM_INIT_VALUE, total_image_size);
          #endif
          #if (defined(CAMERA_DEBUG) && (CAMERA_DEBUG == 1)) 
              nrf_drv_gpiote_in_event_enable(FRAME_VLD, true);
              APP_ERROR_CHECK(nrf_drv_spis_buffers_set(&spis, m_tx_buf, m_length_tx, m_rx_buf, m_length_rx));
          #endif
        #endif
        image_rd_done = 1;

        if(acc_rec_flag == true){
        //Read the Acc and Mag data and save it at the end of the image
            mc6470_acc_read();
        }

        #if (FINAL_CODE ==0)
        printf("Received all \n");
        #endif

    }

}

void lvld_timer_init(void)
{
    uint32_t err_code = NRF_SUCCESS;
    //Configure TIMER_LED for generating simple light effect - leds on board will invert his state one after the other.
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    err_code = nrf_drv_timer_init(&TIMER_LVLD, &timer_cfg, timer_lvld_event_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_timer_extended_compare(
         &TIMER_LVLD, NRF_TIMER_CC_CHANNEL4, lvld_timer_val, NRF_TIMER_SHORT_COMPARE4_CLEAR_MASK, true);

    //nrf_drv_timer_enable(&TIMER_LVLD);
}

/** @} */
