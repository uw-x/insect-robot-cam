
 /** @file
 *
 *  Created: April 05, 2019
 *      Author: Ali Najafi
 */



#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "app_error.h"
#include "bsp.h"
#include "nrf_delay.h"
#include "app_pwm.h"

#include "HM01B0_BLE_DEFINES.h"

//extern uint32_t DUTY_VALUES_PWM_BOOST[DUTY_BOOST_ARRAY_SIZE] = {5, 15, 20, 40, 80, 100, 200, 300, 500};
//extern uint32_t PERIOD_VALUES_PWM_BOOST[DUTY_BOOST_ARRAY_SIZE] = {320, 160, 160, 80, 40, 40, 20, 20, 10};

extern uint32_t DUTY_VALUES_PWM_BOOST[DUTY_BOOST_ARRAY_SIZE] = {5, 15, 20, 40, 80, 100, 200, 300, 500};
extern uint32_t PERIOD_VALUES_PWM_BOOST[DUTY_BOOST_ARRAY_SIZE] = {320, 160, 160, 80, 40, 40, 20, 20, 10};
extern uint8_t current_boost_num = 0;
//#define P04  NRF_GPIO_PIN_MAP(0,4)

//APP_PWM_INSTANCE(PWM1,2);                   // Create the instance "PWM1" using TIMER3.
APP_PWM_INSTANCE(PWM2,3);                   // Create the instance "PWM2" using TIMER4.

static volatile bool ready_flag;            // A flag indicating PWM status.

void pwm_ready_callback(uint32_t pwm_id);    // PWM callback function

// duty cycle, voltage (V), current (mA)
// 6/100k, 140, 25
// 7/100k, 160, 40
// 8/100k, 180, 55

void pwm_code(uint32_t piezo_period, uint32_t piezo_duty, uint32_t boost_period, uint32_t boost_duty);
void pwm_init(uint32_t boost_period);
void pwm_boost_zero(void);
void pwm_boost_step2(uint32_t boost_duty);
void pwm_boost_step(uint32_t boost_duty, uint8_t steps, uint8_t delay);
void pwm_boost_step_rise_fall(uint32_t boost_duty, uint8_t steps_up, uint8_t steps_down, uint8_t delay, bool polarity);

void pwm_boost_step_2CH(uint32_t boost_duty, uint8_t steps, uint8_t delay);
void pwm_boost_ramp(uint32_t boost_duty, uint8_t delay);


/*Table of duty-cycle, voltage, current[uA]
Seems like the ratio of the duty-cycle/boost_period should higher than 10
These values change
boost_period = 10
500-222-18

boost_period = 20
300-196-9.4
200-170-5.7

boost_period = 40
100-140-2.9
80-108-1.4

boost_period = 80
40-86-740

boost_period = 160
20-66-387
15-40-119

boost_period = 320
5-30-64

*/

/** @} */
