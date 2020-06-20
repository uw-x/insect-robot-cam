/*
 * HM01B0_GPIO.c
 *
 *  Created on: Nov 20, 2018
 *      Author: Ali Najafi
 */


#include "HM01B0_GPIO.h"
extern uint8_t m_new_command_received = 0;
/*******************************************************************************
 * New Code for GPIO using SPI
 ******************************************************************************/

/*!
* @brief GPIO settings initialization.
*/



void in_pin_handler_line_vld(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
//    nrf_drv_gpiote_in_event_disable(LINE_VLD);
    if (line_count<LINE_NUM){
    // here we need to activate SPI CS; enable the lvld_timer; and activate the line_vld interrupt; increase the counter of lines
        lvld_timer_run(&TIMER_LVLD);        
        gpio_p_reg->OUTCLR = CAM_SPI_PIN_MASK;
        line_count++;
//        nrf_drv_gpiote_in_event_enable(LINE_VLD, true);
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

        #if (FINAL_CODE ==0)
        printf("Received all \n");
        #endif
    }
}


void in_pin_handler_frame_vld(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    nrf_drv_gpiote_in_event_disable(FRAME_VLD);

    line_count = 0;
    m_length_rx_done = 0;
    lvld_timer_run(&TIMER_LVLD);
    nrf_gpio_pin_clear(CAM_SPI_CS);
//    spis_xfer_done = true;
}

void in_pin_handler_acc_int(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    nrf_drv_gpiote_in_event_disable(ACC_INT_PIN);
    
    if(acc_rec_flag == true){
        acc_rec_counter = 0;
        packet_sent_acc = 0;
        //Read the Acc and Mag data and save it at the end of the image
        mc6470_acc_read();
    }
    #if(CAM_CLK_GATING == 1)
    nrf_drv_timer_enable(&CAM_TIMER);
    #endif
    accel_sr = hm_i2c_read_8b(SR, SLAV_ADDR_ACC);
//    acc_int_flag = true;
    if(!acc_int_cmd_sweep){
        acc_pic_num = ACC_PIC_CNT;
    } else{
        acc_pic_num = ACC_PIC_CNT_SWEEP;
    }

    m_stream_mode_active = true;


    NRF_LOG_INFO("Capture complete: size %i bytes", (uint32_t)(m_length_rx_done));

    #if (RELEASE_CODE ==0)
        printf("Capture complete: size %i bytes\n", (uint32_t)(m_length_rx_done));
    #endif
    

//    single_capture_flag = 1;

}

void gpio_setting_uninit(void)
{

   
    nrf_drv_gpiote_in_uninit(FRAME_VLD);
    

    nrf_drv_gpiote_in_uninit(LINE_VLD);
    

    nrf_drv_gpiote_in_uninit(ACC_INT_PIN);
    
}


void gpio_setting_init(void)
{

//    gpio_p_reg->OUTSET = CAM_SPI_PIN_MASK;
//    gpio_p_reg->OUTCLR = CAM_SPI_PIN_MASK;
    ret_code_t err_code;

    /*gpiote initialization has been done in clock and timer initialization, no need to do it again*/
//    err_code = nrf_drv_gpiote_init();
//    APP_ERROR_CHECK(err_code);

//    nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_SIMPLE(false);
//
//    err_code = nrf_drv_gpiote_out_init(PIN_OUT, &out_config);
//    APP_ERROR_CHECK(err_code);

    /*Finds rising edge instead of just toggling*/
    nrf_drv_gpiote_in_config_t in_config_frmvld = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
//    nrf_drv_gpiote_in_config_t in_config = NRFX_GPIOTE_RAW_CONFIG_IN_SENSE_LOTOHI(true);
    in_config_frmvld.pull = NRF_GPIO_PIN_NOPULL;

    err_code = nrf_drv_gpiote_in_init(FRAME_VLD, &in_config_frmvld, in_pin_handler_frame_vld);
    APP_ERROR_CHECK(err_code);


    /*Finds falling for line valid edge instead of just toggling*/
    nrf_drv_gpiote_in_config_t in_config_lnvld = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
//    nrf_drv_gpiote_in_config_t in_config = NRFX_GPIOTE_RAW_CONFIG_IN_SENSE_LOTOHI(true);
    in_config_lnvld.pull = NRF_GPIO_PIN_NOPULL;

    err_code = nrf_drv_gpiote_in_init(LINE_VLD, &in_config_lnvld, in_pin_handler_line_vld);
    APP_ERROR_CHECK(err_code);


    /*Find accelerometer interrupt falling edge*/
    nrf_drv_gpiote_in_config_t in_config_accint = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
//    nrf_drv_gpiote_in_config_t in_config = NRFX_GPIOTE_RAW_CONFIG_IN_SENSE_LOTOHI(true);
    in_config_accint.pull = NRF_GPIO_PIN_NOPULL;

    err_code = nrf_drv_gpiote_in_init(ACC_INT_PIN, &in_config_accint, in_pin_handler_acc_int);
    APP_ERROR_CHECK(err_code);

    nrf_gpio_cfg(
    BOOST_PNP_SWITCH,
    NRF_GPIO_PIN_DIR_OUTPUT,
    NRF_GPIO_PIN_INPUT_DISCONNECT,
    NRF_GPIO_PIN_NOPULL,
//    NRF_GPIO_PIN_H0H1,
    NRF_GPIO_PIN_S0S1,
    NRF_GPIO_PIN_NOSENSE);

    nrf_gpio_cfg(
    BOOST_NPN_SWITCH,
    NRF_GPIO_PIN_DIR_OUTPUT,
    NRF_GPIO_PIN_INPUT_DISCONNECT,
    NRF_GPIO_PIN_NOPULL,
//    NRF_GPIO_PIN_H0H1,
    NRF_GPIO_PIN_S0S1,
    NRF_GPIO_PIN_NOSENSE);

    /*This function better to be written in the capture function*/
//    nrf_drv_gpiote_in_event_enable(PIN_IN, true);
}
