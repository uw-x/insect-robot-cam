
 /** @file
 *
 *  Created: April 05, 2019
 *      Author: Ali Najafi
 */


#include "HM01B0_PWM.h"




void pwm_ready_callback(uint32_t pwm_id)    // PWM callback function
{
    ready_flag = true;
}
// duty cycle, voltage (V), current (mA)
// 6/100k, 140, 25
// 7/100k, 160, 40
// 8/100k, 180, 55

void pwm_code(uint32_t piezo_period, uint32_t piezo_duty, uint32_t boost_period, uint32_t boost_duty)
{
    ret_code_t err_code;

//    /* 2-channel PWM, 200Hz, output on DK LED pins. */ 
//    app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_1CH(piezo_period, PIEZO_PIN);
//
//    /* Switch the polarity of the second channel. */
//    pwm1_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;

    //app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_2CH(50L, SER_CON_SPIS_MOSI_PIN, BSP_LED_1);
    app_pwm_config_t pwm2_cfg = APP_PWM_DEFAULT_CONFIG_1CH(boost_period, BOOST_PIN);
    pwm2_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;

    /* Initialize and enable PWM. */
//    err_code = app_pwm_init(&PWM1,&pwm1_cfg,pwm_ready_callback);
//    APP_ERROR_CHECK(err_code);
//    app_pwm_enable(&PWM1);

    err_code = app_pwm_init(&PWM2,&pwm2_cfg,pwm_ready_callback);
    APP_ERROR_CHECK(err_code);
    app_pwm_enable(&PWM2);

//    while (app_pwm_channel_duty_set(&PWM1, 0, piezo_duty) == NRF_ERROR_BUSY);
//    while (app_pwm_channel_duty_set(&PWM1, 1, piezo_duty) == NRF_ERROR_BUSY);
    while (app_pwm_channel_duty_set(&PWM2, 0, boost_duty) == NRF_ERROR_BUSY);

        nrf_gpio_cfg(
    BOOST_PIN,
    NRF_GPIO_PIN_DIR_OUTPUT,
    NRF_GPIO_PIN_INPUT_DISCONNECT,
    NRF_GPIO_PIN_NOPULL,
    NRF_GPIO_PIN_H0H1,
//NRF_GPIO_PIN_S0S1,
    NRF_GPIO_PIN_NOSENSE);

}

void pwm_init(uint32_t boost_period)
{
    ret_code_t err_code;

    app_pwm_config_t pwm2_cfg = APP_PWM_DEFAULT_CONFIG_1CH(boost_period, BOOST_PIN);
    pwm2_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;

    /* Initialize and enable PWM. */
    err_code = app_pwm_init(&PWM2,&pwm2_cfg,pwm_ready_callback);
    APP_ERROR_CHECK(err_code);
    app_pwm_enable(&PWM2);

}

void pwm_boost_step2(uint32_t boost_duty)
{
    while (app_pwm_channel_duty_set(&PWM2, 0, boost_duty) == NRF_ERROR_BUSY);
}


void pwm_boost_step(uint32_t boost_duty, uint8_t steps, uint8_t delay)
{
    uint8_t i=0;
    while(DUTY_VALUES_PWM_BOOST[i] < boost_duty){
    i++;
    }

    current_boost_num = i+1;//0 corresponds to uninit case

    ret_code_t err_code;

    app_pwm_config_t pwm2_cfg = APP_PWM_DEFAULT_CONFIG_1CH(PERIOD_VALUES_PWM_BOOST[i], BOOST_PIN);
    pwm2_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;

    /* Initialize and enable PWM. */
    err_code = app_pwm_init(&PWM2,&pwm2_cfg,pwm_ready_callback);
    APP_ERROR_CHECK(err_code);
    app_pwm_enable(&PWM2);

    uint8_t j = 0;
    uint8_t steps_new = MIN(steps,i);
    while(j<=steps_new){
        while (app_pwm_channel_duty_set(&PWM2, 0, (uint32_t) j*boost_duty/steps_new) == NRF_ERROR_BUSY);
        nrf_delay_ms(delay);
        j++;
    }

    nrf_gpio_cfg(
    BOOST_PIN,
    NRF_GPIO_PIN_DIR_OUTPUT,
    NRF_GPIO_PIN_INPUT_DISCONNECT,
    NRF_GPIO_PIN_NOPULL,
    NRF_GPIO_PIN_H0H1,
//NRF_GPIO_PIN_S0S1,
    NRF_GPIO_PIN_NOSENSE);

}

void pwm_boost_zero(void){
//    pwm_boost_step_rise_fall(DUTY_VALUES_PWM_BOOST[0], PWM_STEPS_NUM, PWM_STEPS_NUM_DOWN , PWM_STEP_DELAY, true);
//    pwm_boost_step_rise_fall(0, PWM_STEPS_NUM, PWM_STEPS_NUM_DOWN , PWM_STEP_DELAY, false);
    app_pwm_uninit(&PWM2);
//    nrf_delay_ms(4000);
    current_boost_num = 0;
    nrf_gpio_cfg(
    BOOST_PIN,
    NRF_GPIO_PIN_DIR_OUTPUT,
    NRF_GPIO_PIN_INPUT_DISCONNECT,
    NRF_GPIO_PIN_PULLDOWN,
    NRF_GPIO_PIN_H0H1,
    NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_pin_clear(BOOST_PIN);
// Discharge to zero
//    nrf_gpio_pin_set(BOOST_PNP_SWITCH);
//    nrf_gpio_pin_set(BOOST_NPN_SWITCH);
//    nrf_delay_ms(400);
    nrf_gpio_pin_clear(BOOST_PNP_SWITCH);
    nrf_gpio_pin_clear(BOOST_NPN_SWITCH);

//    nrf_delay_ms(200);
//    uint8_t i=9;
//    while(i >= 0){
////        nrf_gpio_pin_set(BOOST_PNP_SWITCH);
//        nrf_gpio_pin_set(BOOST_NPN_SWITCH);
//        nrf_delay_ms(1);
//        nrf_gpio_pin_clear(BOOST_PNP_SWITCH);
//        nrf_gpio_pin_clear(BOOST_NPN_SWITCH);
//        nrf_delay_ms(200);
//        i--;
//    }
}

void pwm_boost_step_rise_fall(uint32_t boost_duty, uint8_t steps_up, uint8_t steps_down, uint8_t delay, bool polarity)
{
    /*First we should step down from the previous angle*/
//    uint8_t i = current_boost_num - 1;
    uint8_t i = steps_down - 1;
    if(current_boost_num > 0){
        while(i >= 2){
            while (app_pwm_channel_duty_set(&PWM2, 0, (uint32_t) (i-1)*DUTY_VALUES_PWM_BOOST[current_boost_num - 1]/steps_down) == NRF_ERROR_BUSY);
            nrf_delay_ms(delay);
            i--;
        }
        app_pwm_uninit(&PWM2);
    }

    nrf_gpio_pin_set(BOOST_PNP_SWITCH);
    nrf_gpio_pin_set(BOOST_NPN_SWITCH);
    nrf_delay_ms(10);
    nrf_gpio_pin_clear(BOOST_PNP_SWITCH);
    nrf_gpio_pin_clear(BOOST_NPN_SWITCH);

        if(polarity){
        nrf_gpio_pin_set(BOOST_PNP_SWITCH);
        nrf_gpio_pin_clear(BOOST_NPN_SWITCH);
    } else {
        nrf_gpio_pin_clear(BOOST_PNP_SWITCH);
        nrf_gpio_pin_set(BOOST_NPN_SWITCH);
    }
    
    i = 0;
    while(DUTY_VALUES_PWM_BOOST[i] < boost_duty){
    i++;
    }

    current_boost_num = i+1;//0 corresponds to uninit case

    ret_code_t err_code;


        

    app_pwm_config_t pwm2_cfg = APP_PWM_DEFAULT_CONFIG_1CH(PERIOD_VALUES_PWM_BOOST[i], BOOST_PIN);
    pwm2_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;

    /* Initialize and enable PWM. */
    err_code = app_pwm_init(&PWM2,&pwm2_cfg,pwm_ready_callback);
    APP_ERROR_CHECK(err_code);
    app_pwm_enable(&PWM2);

    uint8_t j = 0;
//    uint8_t steps_new = MIN(steps,i);
    uint8_t steps_new = steps_up;
    while(j<=steps_new){
        while (app_pwm_channel_duty_set(&PWM2, 0, (uint32_t) j*boost_duty/steps_new) == NRF_ERROR_BUSY);
        nrf_delay_ms(delay);
        j++;
    }

    nrf_gpio_cfg(
    BOOST_PIN,
    NRF_GPIO_PIN_DIR_OUTPUT,
    NRF_GPIO_PIN_INPUT_DISCONNECT,
    NRF_GPIO_PIN_NOPULL,
    NRF_GPIO_PIN_H0H1,
    NRF_GPIO_PIN_NOSENSE);

    if(polarity){
        nrf_gpio_pin_set(BOOST_PNP_SWITCH);
        nrf_gpio_pin_clear(BOOST_NPN_SWITCH);
    } else {
        nrf_gpio_pin_clear(BOOST_PNP_SWITCH);
        nrf_gpio_pin_set(BOOST_NPN_SWITCH);
    }

}

void pwm_boost_step_2CH(uint32_t boost_duty, uint8_t steps, uint8_t delay)
{
    uint8_t i=0;
    while(DUTY_VALUES_PWM_BOOST[i] < boost_duty){
    i++;
    }

    ret_code_t err_code;

    app_pwm_config_t pwm2_cfg = APP_PWM_DEFAULT_CONFIG_2CH(PERIOD_VALUES_PWM_BOOST[i], BOOST_PIN, BOOST_PIN2);
    pwm2_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;
    pwm2_cfg.pin_polarity[1] = APP_PWM_POLARITY_ACTIVE_LOW;

    /* Initialize and enable PWM. */
    err_code = app_pwm_init(&PWM2,&pwm2_cfg,pwm_ready_callback);
    APP_ERROR_CHECK(err_code);
    app_pwm_enable(&PWM2);

    i=1;
    while(i<=steps){
        while (app_pwm_channel_duty_set(&PWM2, 0, (uint32_t) i*boost_duty/steps) == NRF_ERROR_BUSY);
        while (app_pwm_channel_duty_set(&PWM2, 1, (uint32_t) i*boost_duty/steps) == NRF_ERROR_BUSY);
        nrf_delay_ms(delay);
        i++;
    }

    nrf_gpio_cfg(
    BOOST_PIN,
    NRF_GPIO_PIN_DIR_OUTPUT,
    NRF_GPIO_PIN_INPUT_DISCONNECT,
    NRF_GPIO_PIN_NOPULL,
    NRF_GPIO_PIN_H0H1,
//NRF_GPIO_PIN_S0S1,
    NRF_GPIO_PIN_NOSENSE);

    nrf_gpio_cfg(
    BOOST_PIN2,
    NRF_GPIO_PIN_DIR_OUTPUT,
    NRF_GPIO_PIN_INPUT_DISCONNECT,
    NRF_GPIO_PIN_NOPULL,
    NRF_GPIO_PIN_H0H1,
//NRF_GPIO_PIN_S0S1,
    NRF_GPIO_PIN_NOSENSE);

}

void pwm_boost_ramp(uint32_t boost_duty, uint8_t delay)//does not work
{
    ret_code_t err_code;

    uint8_t i=0;
    while(DUTY_VALUES_PWM_BOOST[i] <= boost_duty){
    i++;
    }
    
    uint8_t j=0;
    while(j<=i){
        app_pwm_config_t pwm2_cfg = APP_PWM_DEFAULT_CONFIG_1CH(PERIOD_VALUES_PWM_BOOST[j], BOOST_PIN);
        pwm2_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;

        /* Initialize and enable PWM. */
        err_code = app_pwm_init(&PWM2,&pwm2_cfg,pwm_ready_callback);
        APP_ERROR_CHECK(err_code);
        app_pwm_enable(&PWM2);

        while (app_pwm_channel_duty_set(&PWM2, 0, DUTY_VALUES_PWM_BOOST[j]) == NRF_ERROR_BUSY);
        nrf_delay_ms(delay);
        j++;
    }

    while (app_pwm_channel_duty_set(&PWM2, 0, boost_duty) == NRF_ERROR_BUSY);


}



/** @} */
