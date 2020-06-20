/*
 * HM01B0_CAPTURE.c
 *
 *  Created on: Nov 25, 2018
 *      Author: Ali Najafi
 */

#include "HM01B0_CAPTURE.h"

uint8_t last_memory_value;

#if defined(BOARD_PCA10040)

#endif


void cam_power_up(void){
    nrf_gpio_cfg(
    CAM_POWER,
    NRF_GPIO_PIN_DIR_OUTPUT,
    NRF_GPIO_PIN_INPUT_DISCONNECT,
    NRF_GPIO_PIN_NOPULL,
    NRF_GPIO_PIN_H0H1,
//NRF_GPIO_PIN_S0S1,
    NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_pin_clear(CAM_POWER);   
    nrf_delay_ms(POR_DELAY);
    nrf_gpio_pin_set(CAM_POWER);
    nrf_delay_ms(POR_DELAY);
    
}


void hm01b0_init(void){
    /*Test if camera is functional*/
    hm_i2c_write(REG_MODE_SELECT,0x00);
    uint8_t version = hm_i2c_read(REG_MODEL_ID_L);
    if(version != 0xB0){
        printf("REG_MODEL_ID_L: %x \n", version);
        printf("Camera version problem \n");
    }
    hm_i2c_write( REG_MODE_SELECT, 0x00);//go to stand by mode

    /*Initialize and set high the SPI chip select*/
    nrf_gpio_cfg_output(CAM_SPI_CS);//Set up the chip select for SPI
    nrf_gpio_pin_set(CAM_SPI_CS);


    /*Camera settings initialization*/
    hm01b0_init_fixed_rom_qvga_fixed();
//    hm01b0_init_brighter();
}


/*Deactivates the peripherals that change the power consumption*/
void hm_peripheral_uninit(void){


    nrfx_spis_uninit(&spis);

//    nrfx_gpiote_out_uninit(HM_CLK);
//    nrfx_timer_uninit(&CAM_TIMER);
//    nrf_drv_ppi_uninit();

//    gpio_setting_uninit(); //uinitializing this does not save power

//    nrfx_timer_uninit(&TIMER_LVLD); //uinitializing this does not save power

}


void hm_peripheral_init(void){
    
//    spi_init();

    spis_pin_set();

    hm_clk_out();


    /*Initialize the GPIO settings: Frame valid for */
    gpio_setting_init();


    /*Initialize the I2C for camera*/
    twi_init();
    nrf_delay_ms(100);

    hm01b0_init();

    lvld_timer_init();

    #if(CAM_CLK_GATING == 1)
    nrf_drv_timer_disable(&CAM_TIMER);
    #endif
    

}

/*Activates the peripherals that change the power consumption, when connected and before taking picture*/
void hm_peripheral_connected_init(void){
    
    spi_init();

//    nrf_delay_ms(100);
//    hm_clk_out();


    /*Initialize the GPIO settings: Frame valid for */
//    gpio_setting_init();//uinitializing this does not save power


//    lvld_timer_init();//uinitializing this does not save power

//    #if(CAM_CLK_GATING == 1)
//    nrf_drv_timer_disable(&CAM_TIMER);
//    #endif
    
}

void hm_single_capture_spi_832_stream(void){


    //(*timer0_prescaler_address) = timer0_prescaler0;
    //nrf_delay_ms(100);

    m_length_rx_done = 0;



    /*SPI registers initilization*/
    spis_xfer_done = false;
    #if (MEM_INIT == 1)
      memset(m_rx_buf, MEM_INIT_VALUE, total_spi_buffer_size);
    #endif

    APP_ERROR_CHECK(nrf_drv_spis_buffers_set(&spis, m_tx_buf, m_length_tx, m_rx_buf, m_length_rx));


    /*Camera values initialized*/
//    hm_pixel_counter = 0;
//    msb_flag = 0;
    image_rd_done = 0;
//    ble_bytes_sent_counter = 0;
   

    /*Enable the FRAME VALID interrupt*/
    
    nrf_drv_gpiote_in_event_enable(FRAME_VLD, true);
//    nrf_drv_gpiote_in_event_enable(LINE_VLD, true);


    /*Count the number of frames sent out*/ 
    #if (defined(CAMERA_DEBUG) && (CAMERA_DEBUG == 1) && (FINAL_CODE == 0))
        uint8_t frame_cnt = hm_i2c_read(REG_FRAME_COUNT);
        printf("Initial number of frames: %d \n", frame_cnt);
    #endif
    //printf("Initial number of frames: %d \n", frame_cnt);

//    lvld_timer_run(&TIMER_LVLD);
//    nrf_gpio_pin_clear(CAM_SPI_CS);
    

    #if (BLE_TEST_PIN_USED == 1)
    nrf_gpio_pin_set(BLE_START_PIN);
    #endif
    hm_i2c_write( REG_MODE_SELECT, 0x01);
  

//    while (image_rd_done != 1);
//    while (!spis_xfer_done);
//    spis_xfer_done = false;

    
    #if defined(BOARD_PCA10056)
    m_length_rx_done = m_length_rx;
    #endif

    #if defined(BOARD_PCA10040)
    m_length_rx_done = total_image_size;
    #endif
    
    
}


void hm_single_capture_spi_832(void){

    #if(CAM_CLK_GATING == 1)
    nrf_drv_timer_enable(&CAM_TIMER);
    #endif
    //(*timer0_prescaler_address) = timer0_prescaler0;
    //nrf_delay_ms(100);
    m_length_rx_done = 0;
    ble_bytes_sent_counter = 0;
    line_count = 0;


    /*SPI registers initilization*/
    spis_xfer_done = false;
    #if (MEM_INIT == 1)
      memset(m_rx_buf, MEM_INIT_VALUE, total_spi_buffer_size);
    #endif

    APP_ERROR_CHECK(nrf_drv_spis_buffers_set(&spis, m_tx_buf, m_length_tx, m_rx_buf, m_length_rx));


    /*Camera values initialized*/
//    hm_pixel_counter = 0;
//    msb_flag = 0;
    image_rd_done = 0;
    image_frame_done = 0;
    ble_bytes_sent_counter = 0;
   

    /*Enable the FRAME VALID interrupt*/
    
    nrf_drv_gpiote_in_event_enable(FRAME_VLD, true);
//    nrf_drv_gpiote_in_event_enable(LINE_VLD, true);


    /*Count the number of frames sent out*/ 
    #if (defined(CAMERA_DEBUG) && (CAMERA_DEBUG == 1) && (FINAL_CODE == 0))
        uint8_t frame_cnt = hm_i2c_read(REG_FRAME_COUNT);
        printf("Initial number of frames: %d \n", frame_cnt);
    #endif
    //printf("Initial number of frames: %d \n", frame_cnt);

    lvld_timer_run(&TIMER_LVLD);
    nrf_gpio_pin_clear(CAM_SPI_CS);
    

    #if (BLE_TEST_PIN_USED == 1)
    nrf_gpio_pin_set(BLE_START_PIN);
    #endif
    hm_i2c_write( REG_MODE_SELECT, capture_mode);//If we use the 0x03 mode for single capture, the power of camera stays high after capturing one frame
  

    while (image_rd_done != 1);
  //  while (image_frame_done != 1);
    while (!spis_xfer_done);
//    (*timer0_prescaler_address) = timer0_prescaler0;
    spis_xfer_done = false;

//    hm_i2c_write( REG_MODE_SELECT, 0x00);

    #if defined(BOARD_PCA10056)
    m_length_rx_done = m_length_rx;
    #endif

    #if defined(BOARD_PCA10040)
    m_length_rx_done = total_image_size;
    #endif

//    #if(CAM_CLK_GATING == 1)
//    nrf_drv_timer_disable(&CAM_TIMER);
//    #endif
 
}

void hm_single_capture_spi_832_compressed_stream(void){
    
    #if(CAM_CLK_GATING == 1)
    nrf_drv_timer_enable(&CAM_TIMER);
    #endif
    //(*timer0_prescaler_address) = timer0_prescaler0;
    //nrf_delay_ms(100);
    m_length_rx_done = 0;
    ble_bytes_sent_counter = 0;
    line_count = 0;


    /*SPI registers initilization*/
    spis_xfer_done = false;
    #if (MEM_INIT == 1)
      memset(m_rx_buf, MEM_INIT_VALUE, total_spi_buffer_size);
    #endif

    APP_ERROR_CHECK(nrf_drv_spis_buffers_set(&spis, m_tx_buf, m_length_tx, m_rx_buf, m_length_rx));


    /*Camera values initialized*/
//    hm_pixel_counter = 0;
//    msb_flag = 0;
    image_rd_done = 0;
    image_frame_done = 0;
    ble_bytes_sent_counter = 0;
   

    /*Enable the FRAME VALID interrupt*/
    
    nrf_drv_gpiote_in_event_enable(FRAME_VLD, true);
//    nrf_drv_gpiote_in_event_enable(LINE_VLD, true);


    /*Count the number of frames sent out*/ 
    #if (defined(CAMERA_DEBUG) && (CAMERA_DEBUG == 1) && (FINAL_CODE == 0))
        uint8_t frame_cnt = hm_i2c_read(REG_FRAME_COUNT);
        printf("Initial number of frames: %d \n", frame_cnt);
    #endif
    //printf("Initial number of frames: %d \n", frame_cnt);
    

    #if (BLE_TEST_PIN_USED == 1)
    nrf_gpio_pin_set(BLE_START_PIN);
    #endif
    hm_i2c_write( REG_MODE_SELECT, capture_mode);//If we use the 0x03 mode for single capture, the power of camera stays high after capturing one frame
  

    #if defined(BOARD_PCA10056)
    m_length_rx_done = m_length_rx;
    #endif

    #if defined(BOARD_PCA10040)
    m_length_rx_done = total_image_size;
    #endif

    
}


void hm_single_capture_spi_832_compressed(void){
    
    #if(CAM_CLK_GATING == 1)
    nrf_drv_timer_enable(&CAM_TIMER);
    #endif
    //(*timer0_prescaler_address) = timer0_prescaler0;
    //nrf_delay_ms(100);
    m_length_rx_done = 0;
    ble_bytes_sent_counter = 0;
    line_count = 0;


    /*SPI registers initilization*/
    spis_xfer_done = false;
    #if (MEM_INIT == 1)
      memset(m_rx_buf, MEM_INIT_VALUE, total_spi_buffer_size);
    #endif

    APP_ERROR_CHECK(nrf_drv_spis_buffers_set(&spis, m_tx_buf, m_length_tx, m_rx_buf, m_length_rx));


    /*Camera values initialized*/
//    hm_pixel_counter = 0;
//    msb_flag = 0;
    image_rd_done = 0;
    image_frame_done = 0;
    ble_bytes_sent_counter = 0;
   

    /*Enable the FRAME VALID interrupt*/
    
    nrf_drv_gpiote_in_event_enable(FRAME_VLD, true);
//    nrf_drv_gpiote_in_event_enable(LINE_VLD, true);


    /*Count the number of frames sent out*/ 
    #if (defined(CAMERA_DEBUG) && (CAMERA_DEBUG == 1) && (FINAL_CODE == 0))
        uint8_t frame_cnt = hm_i2c_read(REG_FRAME_COUNT);
        printf("Initial number of frames: %d \n", frame_cnt);
    #endif
    //printf("Initial number of frames: %d \n", frame_cnt);

    lvld_timer_run(&TIMER_LVLD);
    nrf_gpio_pin_clear(CAM_SPI_CS);
    

    #if (BLE_TEST_PIN_USED == 1)
    nrf_gpio_pin_set(BLE_START_PIN);
    #endif
    hm_i2c_write( REG_MODE_SELECT, capture_mode);//If we use the 0x03 mode for single capture, the power of camera stays high after capturing one frame
  

    while (image_rd_done != 1);
    while (!spis_xfer_done);
    spis_xfer_done = false;

    #if defined(BOARD_PCA10056)
    m_length_rx_done = m_length_rx;
    #endif

    #if defined(BOARD_PCA10040)
    m_length_rx_done = total_image_size;
    #endif
//    uint8_t xtest;
    compressed_size = main_compression(quality);
//    hm_i2c_write( REG_MODE_SELECT, 0x00);
    
}





void hm_single_capture(void){

    /*Camera values initialized*/
//    hm_pixel_counter = 0;
//    msb_flag = 0;
//    image_rd_done = 0;
    ble_bytes_sent_counter = 0;
    
    /*Camera settings initialization*/
    hm01b0_init_fixed_rom_qvga_fixed();

    /*Enable the PCLK interrupt function*/
//    nrf_drv_gpiote_in_event_enable(PIN_IN, true);

    /*Count the number of frames sent out*/
    #if (defined(CAMERA_DEBUG) && (CAMERA_DEBUG == 1))
        uint8_t frame_cnt = hm_i2c_read(REG_FRAME_COUNT);
        printf("Initial number of frames: %d \n", frame_cnt);
    #endif



    /*Capture and stream one frame out*/
    hm_i2c_write( REG_MODE_SELECT, capture_mode);

}


void hm_single_capture_spi(void){
    
    m_length_rx_done = 0;
    ble_bytes_sent_counter = 0;

    /*SPI registers initilization*/
    spis_xfer_done = false;
//    memset(m_rx_buf, 0, m_length_rx);
    APP_ERROR_CHECK(nrf_drv_spis_buffers_set(&spis, m_tx_buf, m_length_tx, m_rx_buf, m_length_rx));

    nrf_gpio_pin_clear(CAM_SPI_CS);


    /*Camera values initialized*/
//    hm_pixel_counter = 0;
//    msb_flag = 0;
//    image_rd_done = 0;
    ble_bytes_sent_counter = 0;
   

    /*Enable the FRAME VALID interrupt*/
    nrf_drv_gpiote_in_event_enable(FRAME_VLD, true);

    /*Count the number of frames sent out*/
    #if (defined(CAMERA_DEBUG) && (CAMERA_DEBUG == 1) && (FINAL_CODE == 0))
        uint8_t frame_cnt = hm_i2c_read(REG_FRAME_COUNT);
        printf("Initial number of frames: %d \n", frame_cnt);
    #endif


    /*Capture and stream one frame out*/
    hm_i2c_write( REG_MODE_SELECT, capture_mode);

//    while (!spis_xfer_done);
//    nrf_gpio_pin_set(CAM_D0);

//    while (*(m_rx_buf+spi_buffer_size-325)!=0);
//    while (*(0x2000F9B7)!=0);
//    last_memory_value=*(m_rx_buf+spi_buffer_size);
//    printf('last memory value %x', last_memory_value);
//    nrf_gpio_pin_set(CAM_SPI_CS);
    while (!spis_xfer_done);
//    spis_xfer_done = false;
    m_length_rx_done = m_length_rx;
//    nrf_gpio_pin_set(CAM_D0);
    
}

