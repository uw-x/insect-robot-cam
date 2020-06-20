/*
 * MC6470_FUNC.c
 *
 *  Created on: Apr 26, 2019
 *      Author: Ali Najafi
 */

#include "MC6470_FUNC.h"

void mc6470_mag_init(void)
{
    /*hm_i2c_write_8b(CNTL1,0x88, SLAV_ADDR_MAG);//set the reading to 10Hz
    uint8_t test = hm_i2c_read_8b(STB, SLAV_ADDR_MAG);
    if(test != 0x55){
        printf("STB: %x \n", test);
        printf("Accelerator STB problem \n");
    }*/
    uint8_t test_regs[6] = {0x10, 0x11, 0x12, 0x14, 0x1C, 0x1D};
    uint8_t test = 0;
    for(int i=0; i<6; i++){
       test = hm_i2c_read_8b(test_regs[i], 0x1C);
       printf("%x: ", test_regs[i]);
       printf("%x\n", test);
    }
    
    hm_i2c_write_8b(0x12,0x00, 0x1C);//set the reading to 10Hz
    nrf_delay_ms(1);
    hm_i2c_write_8b(0x1C,0x80, 0x1C);//set the reading to 10Hz
    nrf_delay_ms(1);
    hm_i2c_write_8b(0x10,0xDF, 0x1C);//set the reading to 10Hz
    nrf_delay_ms(1);

    for(int i=0; i<6; i++){
       test = hm_i2c_read_8b(test_regs[i], 0x1C);
       printf("%x: ", test_regs[i]);
       printf("%x\n", test);
    }

    /*test = hm_i2c_read_8b(WHO_AM_I, SLAV_ADDR_MAG);
    if(test != 0x49){
        printf("WHO AM I: %x \n", test);
        printf("Accelerator WHO AM I problem \n");
    }
    */

    //Seems that in the normal state, we don't need to calibrate. As a matter of fact, the IC does not let you set OCL in the normal state!

//    hm_i2c_write_8b(CNTL3,0x01, SLAV_ADDR_MAG);
//
//    test = hm_i2c_read_8b(OFFSET_X_LSB, SLAV_ADDR_MAG);
//
//    test = hm_i2c_read_8b(OFFSET_X_MSB, SLAV_ADDR_MAG);
//
//    test = hm_i2c_read_8b(OFFSET_Y_LSB, SLAV_ADDR_MAG);
//
//    test = hm_i2c_read_8b(OFFSET_Y_MSB, SLAV_ADDR_MAG);
//
//    test = hm_i2c_read_8b(OFFSET_Z_LSB, SLAV_ADDR_MAG);
//
//    test = hm_i2c_read_8b(OFFSET_Z_MSB, SLAV_ADDR_MAG);



}


void acc_power_up(void){
    nrf_gpio_cfg(
    ACC_POWER_VALUE,
    NRF_GPIO_PIN_DIR_OUTPUT,
    NRF_GPIO_PIN_INPUT_DISCONNECT,
    NRF_GPIO_PIN_PULLUP,
//    NRF_GPIO_PIN_H0H1,
NRF_GPIO_PIN_S0S1,
    NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_pin_clear(ACC_POWER_VALUE);
    nrf_delay_ms(POR_DELAY);
    nrf_gpio_pin_set(ACC_POWER_VALUE);
    nrf_delay_ms(POR_DELAY);
}

void acc_power_down(void){

    hm_i2c_write_8b(MODE, 0x00, SLAV_ADDR_ACC);
    nrf_gpio_cfg(
    ACC_POWER_VALUE,
    NRF_GPIO_PIN_DIR_OUTPUT,
    NRF_GPIO_PIN_INPUT_DISCONNECT,
    NRF_GPIO_PIN_NOPULL,
//    NRF_GPIO_PIN_H0H1,
    NRF_GPIO_PIN_S0S1,
    NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_pin_clear(ACC_POWER_VALUE);
    nrf_delay_ms(POR_DELAY);
}



void mc6470_acc_init(void){

    acc_power_up(); 

    hm_i2c_write_8b(MODE, 0x00, SLAV_ADDR_ACC);

    hm_i2c_write_8b(SRTFR, 0x00, SLAV_ADDR_ACC);

    hm_i2c_write_8b(INTEN, 0x15, SLAV_ADDR_ACC);
    hm_i2c_write_8b(TAPEN, 0xD5, SLAV_ADDR_ACC);
    

    hm_i2c_write_8b(TTTRX, 0x01, SLAV_ADDR_ACC);
    hm_i2c_write_8b(TTTRY, 0x01, SLAV_ADDR_ACC);
    hm_i2c_write_8b(TTTRZ, 0x0A, SLAV_ADDR_ACC);
    
    hm_i2c_write_8b(OUTCFG, 0x35, SLAV_ADDR_ACC); // set to 8g range, 14 bits
    hm_i2c_write_8b(MODE, WAKE, SLAV_ADDR_ACC);

    accel_sr = hm_i2c_read_8b(SR, SLAV_ADDR_ACC);

    hm_i2c_write_8b(MODE, 0x00, SLAV_ADDR_ACC);//Deactivate the magnetometer

}


void mc6470_acc_read(void){
    if(acc_rec_counter < m_length_rx){
        m_rx_buf[total_image_size + acc_rec_counter] = hm_i2c_read_8b(XOUT_EX_L, SLAV_ADDR_ACC);
        acc_rec_counter++;
        m_rx_buf[total_image_size + acc_rec_counter] = hm_i2c_read_8b(XOUT_EX_H, SLAV_ADDR_ACC);
        acc_rec_counter++;
        m_rx_buf[total_image_size + acc_rec_counter] = hm_i2c_read_8b(YOUT_EX_L, SLAV_ADDR_ACC);
        acc_rec_counter++;
        m_rx_buf[total_image_size + acc_rec_counter] = hm_i2c_read_8b(YOUT_EX_H, SLAV_ADDR_ACC);
        acc_rec_counter++;
        m_rx_buf[total_image_size + acc_rec_counter] = hm_i2c_read_8b(ZOUT_EX_L, SLAV_ADDR_ACC);
        acc_rec_counter++;
        m_rx_buf[total_image_size + acc_rec_counter] = hm_i2c_read_8b(ZOUT_EX_H, SLAV_ADDR_ACC);
        acc_rec_counter++;

        m_rx_buf[total_image_size + acc_rec_counter] = hm_i2c_read_8b(MAG_X_LSB, SLAV_ADDR_MAG);
        acc_rec_counter++;
        m_rx_buf[total_image_size + acc_rec_counter] = hm_i2c_read_8b(MAG_X_MSB, SLAV_ADDR_MAG);
        acc_rec_counter++;
        m_rx_buf[total_image_size + acc_rec_counter] = hm_i2c_read_8b(MAG_Y_LSB, SLAV_ADDR_MAG);
        acc_rec_counter++;
        m_rx_buf[total_image_size + acc_rec_counter] = hm_i2c_read_8b(MAG_Y_MSB, SLAV_ADDR_MAG);
        acc_rec_counter++;
        m_rx_buf[total_image_size + acc_rec_counter] = hm_i2c_read_8b(MAG_Z_LSB, SLAV_ADDR_MAG);
        acc_rec_counter++;
        m_rx_buf[total_image_size + acc_rec_counter] = hm_i2c_read_8b(MAG_Z_MSB, SLAV_ADDR_MAG);
        acc_rec_counter++;
    }
}

