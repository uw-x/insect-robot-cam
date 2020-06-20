/*
 * HM01B0_GPIO.h
 *
 *  Created on: Nov 20, 2018
 *      Author: Ali Najafi
 */

#ifndef HM01B0_GPIO_H_
#define HM01B0_GPIO_H_

/*  Standard C Included Files */
#include <stdbool.h>
//#include <stdio.h>
//#include <string.h>

#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "boards.h"
#include "HM01B0_SPI.h"
#include "HM01B0_LVLD_TIMER.h"
#include "HM01B0_CAPTURE.h"
#include "MC6470_FUNC.h"
//#include "ble_image_transfer_service.h"

/*******************************************************************************
 * GPIO Variables
 ******************************************************************************/


//uint32_t sw_cam_d0;


//XXXXX
#if defined(BOARD_PCA10056)
uint32_t CAM_SPI_CS =  44;
#define FRAME_VLD 43
#endif 

//#if defined(BOARD_PCA10040)
//uint32_t CAM_SPI_CS =  22;
//#define FRAME_VLD 20
//#endif    

#if defined(BOARD_PCA10040)
extern uint32_t CAM_SPI_PIN_MASK = 1UL << CAM_SPI_CS_VALUE;//This parameter and the next line should be changed together and should have the same value
extern uint32_t CAM_SPI_CS =  CAM_SPI_CS_VALUE;//This parameter and the previous line should be changed together and should have the same value
//extern uint32_t CAM_LINE_VLD =  14;
extern uint32_t CAM_POWER =  CAM_POWER_VALUE;
//NRF_GPIO_Type * gpio_p_reg = nrf_gpio_pin_port_decode(&CAM_SPI_CS);

extern uint32_t BLE_START_PIN = BLE_START_PIN_VALUE;
//nrf_gpio_pin_port_decode(&CAM_SPI_CS);
extern NRF_GPIO_Type * gpio_p_reg = NRF_GPIO;
extern uint32_t line_count;
extern uint32_t rxd_ptr = m_rx_buf; 
extern uint8_t image_rd_done = 0;
extern uint8_t image_frame_done = 0;
bool acc_int_flag;
uint8_t acc_pic_num;
bool m_stream_mode_active;

uint16_t total_image_size;
bool acc_rec_flag;
bool acc_int_cmd_sweep;
extern uint8_t acc_rec_counter = 0;
uint16_t packet_sent_acc;

#endif   


//uint32_t CAM_SPI_CS =  44;
//#define FRAME_VLD 43

//
//#define PIN_OUT BSP_LED_0
////#define PIN_IN BSP_BUTTON_0
//#define PIN_IN 43//Port1, pin11

//#define pint_intr_callback in_pin_handler_line_vld

/*******************************************************************************
 * Interrupt Variables
 ******************************************************************************/


/*******************************************************************************
 * GPIO Functions
 ******************************************************************************/
/*!
* @brief GPIO settings initialization.
*/
void gpio_setting_init(void);
void gpio_setting_uninit(void);

uint8_t gpio_read_cam(void);



/*******************************************************************************
 * I2C Functions
 ******************************************************************************/
/*!
* @brief I2C settings initialization.
*/


/*******************************************************************************
 * Code PINT Intr
 ******************************************************************************/

/*!
* @brief Call back for PINT Pin interrupt 0-7.
*/
void in_pin_handler_line_vld(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
void in_pin_handler_frame_vld(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);




#endif /* HM01B0_GPIO_H_ */








///*******************************************************************************
// * GPIO Variables
// ******************************************************************************/
////#define gpio1_byte1_address 0x50000811//0x50000300 + 0x510 + 1(higher byte)
////uint8_t *gpio_address = gpio1_byte1_address;
////uint8_t gpio_read;
//
////uint32_t sw_cam_d0;
//
//uint32_t CAM_SPI_CS =  44;
//#define FRAME_VLD 43
//
////#define CAM_D0	44 //Port1, pin12
////#define CAM_D1	45
////#define CAM_D2	46
////#define CAM_D3	47
////
////#define PIN_OUT BSP_LED_0
//////#define PIN_IN BSP_BUTTON_0
////#define PIN_IN 43//Port1, pin11
//
//#define pint_intr_callback in_pin_handler
//
///*******************************************************************************
// * Interrupt Variables
// ******************************************************************************/
//// #define hm_mem_size 766
////static uint32_t hm_image_size = 766;
////#define hm_mem_size 100
////static uint32_t hm_image_size = 100;
////extern uint8_t	image_mem[hm_mem_size];
////extern uint8_t image_rd_done;
////
////
////extern uint32_t hm_pixel_counter;
////static bool msb_flag; //0 means lsb data, 1 msb data
////uint8_t test_gpio = 0xC5;
////uint8_t mask_lsb = 0x0F;
////uint8_t mask_msb = 0xF0;
////uint8_t gpio_8b;
//
///*******************************************************************************
// * GPIO Functions
// ******************************************************************************/
///*!
//* @brief GPIO settings initialization.
//*/
//void gpio_setting_init(void);
//
//uint8_t gpio_read_cam(void);
//
//
//
///*******************************************************************************
// * I2C Functions
// ******************************************************************************/
///*!
//* @brief I2C settings initialization.
//*/
//
//
///*******************************************************************************
// * Code PINT Intr
// ******************************************************************************/
//
///*!
//* @brief Call back for PINT Pin interrupt 0-7.
//*/
//void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
//
//
//
//
//#endif /* HM01B0_GPIO_H_ */
