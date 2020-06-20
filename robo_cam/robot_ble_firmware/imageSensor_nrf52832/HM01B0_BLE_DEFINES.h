/*
 * HM01B0_BLE_DEFINES.h
 *
 *  Created on: Apr 05, 2019
 *      Author: Ali Najafi
 */

#include <stdint.h>

//Got from HM01B0_SPI.h +++++++++++++++++++++++++++++++++++++++++
#ifndef HM01B0_BLE_DEFINES_H_
#define HM01B0_BLE_DEFINES_H_

//Code shortcuts+++++++++++++++++++++++++++++++++++++++++++++++++
#define lvld_timer_pause  nrf_drv_timer_disable
#define lvld_timer_run  nrf_drv_timer_enable

//#define spi_buffer_size 33000
#define spi_buffer_size 100
#define ROW_INIT 100
#define total_spi_buffer_size_max 32000
#define total_spi_buffer_size 10000
extern uint8_t LINE_NUM;
extern uint16_t total_image_size;


//Got from HM01B0_COMPRESS.h +++++++++++++++++++++++++++++++++++++++++
#define COMPRESSED_FILE_NAME "test_lowest4.jpg"
#define jpeg_imag_mem_size 100
#define NRF_FPU_USED 0
#define JPEG_16B 1
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#define SPIS_INSTANCE 2 /**< SPIS instance index. */


#ifdef BSP_LED_0
    #define PIN_OUT BSP_LED_0
#endif
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//Got from HM01B0_CAPTURE.h +++++++++++++++++++++++++++++++++++++++++
/*if capture_mode=0x01 => it streams. This is useful for tetsing purposes*/
/*if capture_mode=0x03 => it takes frame_count shots. */
#define capture_mode  0x01
#define CAMERA_DEBUG 0
#define BLE_DEBUG 0
#define JPEG_COMPRESS 0//Activates the JPEG stuff
#define FINAL_CODE 1
#define TEST_PATTERN 0x00
#define BLE_TEST_PIN_USED 0
#define MEM_INIT  0
#define MEM_INIT_VALUE 0x11
#define FRAME_VLD_INT 1//Use the frame valid in the code
#define RELEASE_CODE  1//Get rid of the print stuff
#define CAM_CLK_GATING 1//Turns off camera clock when not taking pictures
#define CAM_SINGLE_CAPTURE_POWER_FIX 0//after signle capture the camera power stays high for this reason I power cycle the camera each time
#define POR_DELAY 200
#define CAM_TURN_OFF_DELAY 100
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//PINS USED+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Got from HM01B0_PWM.h 
#define PIEZO_PIN  NRF_GPIO_PIN_MAP(0,24)
#define BOOST_CLK_PIN_VALUE  12
#define BOOST_CLK_PIN_VALUE2  25
#define BOOST_PIN  NRF_GPIO_PIN_MAP(0,BOOST_CLK_PIN_VALUE)
#define BOOST_PIN2  NRF_GPIO_PIN_MAP(0,BOOST_CLK_PIN_VALUE2)
#define TIMER_BOOST_ARRAY_SIZE 21
#define DUTY_BOOST_ARRAY_SIZE 9
#define PERIOD_BOOST_ARRAY_SIZE 6
#define PERIOD_BOOST_ARRAY_SIZE 6
#define pwm_boost_step_func pwm_boost_step
#define BOOST_PNP_SWITCH 8
#define BOOST_NPN_SWITCH 6
#define CAM_SWEEP_REG_FRAME_LENGTH_LINES_H 0x07
#define CAM_SWEEP_REG_FRAME_LENGTH_LINES_L 0x12
#define PWM_STEPS_NUM 200
#define PWM_STEPS_NUM_DOWN 15
#define PWM_STEP_DELAY 1

//Got from HM01B0_CLK.h 
#if defined(BOARD_PCA10056)
#define HM_CLK 15  /**< Pin number for output. */
#endif 

#if defined(BOARD_PCA10040)
#define HM_CLK 11  /**< Pin number for output. */
#endif 

//Got from HM01B0_GPIO.h 
#define BLE_START_PIN_VALUE 31
#define FRAME_VLD 22
#define LINE_VLD 19
//#define CAM_POWER_VALUE 23
#define CAM_POWER_VALUE 18
#define CAM_SPI_CS_VALUE 25
//#define CAM_SPI_CS_VALUE 20

//#define ACC_POWER_VALUE 24
#define ACC_POWER_VALUE 24
#define ACC_INT_PIN 20
#define MAG_INT_PIN 23

#define ACC_PIC_CNT 4
#define ACC_PIC_CNT_SWEEP 4

#define LEFT_EN 17
#define RIGHT_EN 7


//Got from HM01B0_I2C.h 
#define SLAV_ADDR_ACC 0x4C
#define SLAV_ADDR_MAG 0x0C
#define SLAVE_ADDR 0x24U //address for camera
#define I2C_DATA_LENGTH 32U
/* TWI instance ID. */
#define TWI_INSTANCE_ID     0
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define I2C_RST_TIMER_VALUE 2000*8*2
#define I2C_RST_CNT_MAX 8
#define I2C_RST_ENABLE 1

//Got from HM01B0_LVLD_TIMER.h +++++++++++++++++++++++++++++++++++++++++
#define LVLD_TIMER_VALUE (spi_buffer_size + 20)*8*2 //the second number in multiplication is equal to 64/cam_mclk_freq; if cam_mcl_freq=8MHz => 8
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//Timers used +++++++++++++++++++++++++++++++++++++++++
/*Timer 0 => BLE timing  NRF_DRV_TIMER_INSTANCE(0)
Timer 1 => camera clk NRF_DRV_TIMER_INSTANCE(1)
Timer 2 => I2C stuck reset timer!!   NRF_DRV_TIMER_INSTANCE(2) 
Timer 3 => pwm 3  NRF_DRV_TIMER_INSTANCE(3)
Timer 4 => LVLD timer!!   NRF_DRV_TIMER_INSTANCE(4) */ 
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//IRQ priorities +++++++++++++++++++++++++++++++++++++++++
/*Timer 0 => BLE timing
GPIOTE:5
LVLD Timer:3
SPIS_ENABLED: 4
NRFX_SPIS:4
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif /* HM01B0_BLE_DEFINES_H_ */