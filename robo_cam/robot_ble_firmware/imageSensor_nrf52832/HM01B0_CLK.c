/*
 * HM01B0_CLK.h
 *
 *  Created on: Nov 25, 2018
 *      Author: Ali Najafi
 */

#include "HM01B0_CLK.h"
#include "HM01B0_CAPTURE.h"

//#define HM_CLK 15




/*Set timer0 prescaler*/
void set_timer0_prescaler(uint32_t timer0_prescaler){
    /*Setting the prescaler value to divide by 1*/
//    *(uint32_t*)0x40008510=0x00000000;
    uint32_t* timer0_prescaler_address = 0x40008510;
    (*timer0_prescaler_address) = timer0_prescaler;
}


void hm_clk_out(void){
    ret_code_t err_code;

    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code);


//    #if (defined(CAMERA_DEBUG) && (CAMERA_DEBUG == 1))
//        /*gpiote initialization has been done in app_button.c, no need to do it again*/
//        err_code = nrf_drv_gpiote_init();
//        APP_ERROR_CHECK(err_code);
//    #endif

    /*gpiote initialization has been done in app_button.c, no need to do it again*/
//    err_code = nrf_drv_gpiote_init();
//    APP_ERROR_CHECK(err_code);

    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    err_code = nrf_drv_timer_init(&CAM_TIMER, &timer_cfg, timer_dummy_handler);
//    printf("error_code: %x \n", err_code);
    APP_ERROR_CHECK(err_code);

    // Setup PPI channel with event from TIMER compare and task GPIOTE pin toggle.
//    uint32_t* timer0_prescaler_address = 0x40009510;//address for timer1 prescaler
//    uint32_t timer0_prescaler = 0x00000001;
    (*timer0_prescaler_address) = timer0_prescaler0;

    uint32_t compare_evt_addr;
    uint32_t gpiote_task_addr;
    nrf_ppi_channel_t ppi_channel;
//    ret_code_t err_code;
    nrf_drv_gpiote_out_config_t config = GPIOTE_CONFIG_OUT_TASK_TOGGLE(false);

    err_code = nrf_drv_gpiote_out_init(HM_CLK, &config);
    APP_ERROR_CHECK(err_code);


    nrf_drv_timer_extended_compare(&CAM_TIMER, (nrf_timer_cc_channel_t)0, 1UL, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);

    err_code = nrf_drv_ppi_channel_alloc(&ppi_channel);
    APP_ERROR_CHECK(err_code);

    compare_evt_addr = nrf_drv_timer_event_address_get(&CAM_TIMER, NRF_TIMER_EVENT_COMPARE0);
    gpiote_task_addr = nrf_drv_gpiote_out_task_addr_get(HM_CLK);

    err_code = nrf_drv_ppi_channel_assign(ppi_channel, compare_evt_addr, gpiote_task_addr);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_ppi_channel_enable(ppi_channel);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_out_task_enable(HM_CLK);

    // Enable timer

    nrf_drv_timer_enable(&CAM_TIMER);

}