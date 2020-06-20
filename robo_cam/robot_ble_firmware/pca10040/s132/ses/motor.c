/*
 * MC6470Regs.h
 *
 *  Created on: Nov 16, 2019
 *      Author: Vikram 
 */
#include "motor.h"

void rt8092_init(void){
    nrf_gpio_cfg(
    LEFT_EN,
    NRF_GPIO_PIN_DIR_OUTPUT,
    NRF_GPIO_PIN_INPUT_DISCONNECT,
    NRF_GPIO_PIN_PULLDOWN,
    NRF_GPIO_PIN_S0S1,
    NRF_GPIO_PIN_NOSENSE);

    nrf_gpio_cfg(
    RIGHT_EN,
    NRF_GPIO_PIN_DIR_OUTPUT,
    NRF_GPIO_PIN_INPUT_DISCONNECT,
    NRF_GPIO_PIN_PULLDOWN,
    NRF_GPIO_PIN_S0S1,
    NRF_GPIO_PIN_NOSENSE);
   
    stop_robot();

    hm_i2c_write_8b(0x12,0x00, 0x1C);//set the reading to 10Hz
    nrf_delay_ms(1);
    hm_i2c_write_8b(0x1C,0x81, 0x1C);//set the reading to 10Hz
    nrf_delay_ms(1);
    hm_i2c_write_8b(0x10,0xDF, 0x1C);//set the reading to 10Hz
    nrf_delay_ms(1);

    /*// Print regs to debug
    uint8_t regs[6] = {0x11, 0x12, 0x16, 0x1D};
    uint8_t test = 0;
    for(int i=0; i<6; i++){
       test = hm_i2c_read_8b(regs[i], 0x1C);
       printf("%x: ", regs[i]);
       printf("%x\n", test);
    }*/
    //for(int i=0; i<100; i++)
    //   step_forward();
    //stop_robot();

}

void motor_start(uint8_t side){
    if(side){
      nrf_gpio_pin_clear(LEFT_EN);
      hm_i2c_write_8b(0x10,0xFF, 0x1C);//set the reading to 10Hz
      nrf_gpio_pin_set(LEFT_EN);
      nrf_delay_ms(10);
      nrf_gpio_pin_clear(LEFT_EN);
      hm_i2c_write_8b(0x10,0x9F, 0x1C);//set the reading to 10Hz
      nrf_gpio_pin_set(LEFT_EN);
    } else {
      nrf_gpio_pin_clear(RIGHT_EN);
      hm_i2c_write_8b(0x10,0xFF, 0x1C);//set the reading to 10Hz
      nrf_gpio_pin_set(RIGHT_EN);
      nrf_delay_ms(10);
      nrf_gpio_pin_clear(RIGHT_EN);
      hm_i2c_write_8b(0x10,0x9F, 0x1C);//set the reading to 10Hz
      nrf_gpio_pin_set(RIGHT_EN);
    }
}

void motor_stop(uint8_t side){
    if(side){
      nrf_gpio_pin_clear(LEFT_EN);
    } else {
      nrf_gpio_pin_clear(RIGHT_EN);
    }
}

void step_forward(void){
    motor_start(LEFT);
    nrf_delay_ms(200);
    motor_stop(LEFT);

    motor_start(RIGHT);
    nrf_delay_ms(220);
    motor_stop(RIGHT);
    stop_robot();
}

void turn_right(void){
    motor_start(RIGHT);
}

void turn_left(void){
    motor_start(LEFT);
}

void stop_robot(void){
    motor_stop(LEFT);
    motor_stop(RIGHT);
}