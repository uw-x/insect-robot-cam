/*
 * HM01B0_BLE_DEFINES.h
 *
 *  Created on: Apr 20, 2019
 *      Author: Ali
 */
#ifndef HM01B0_PWM_BOOST_H_
#define HM01B0_PWM_BOOST_H_


#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "app_error.h"
#include "bsp.h"
#include "nrf_delay.h"
#include "app_pwm.h"

#include "nrf_drv_timer.h"
#include "HM01B0_BLE_DEFINES.h"


/*Timer part++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
const nrf_drv_timer_t PWM_BOOST_TIMER = NRF_DRV_TIMER_INSTANCE(2);

extern uint32_t pwm_boost_timer_value = 100;
/*lvld_timer_val values and voltage
2 -> 236, 32mA
100->233, 27mA
120->229, 22mA
150>224, 18mA
200->215, 13.6mA
300->198, 9.3mA
450->178
600->164
800->150
1000->140
1250->130
1500->122
2000->110
2500->102
3000->94
4000->84
5000->78
7500->66
10000->58
20000->42
100000->34
*/



extern uint32_t BOOST_CLK_PIN = BOOST_CLK_PIN_VALUE;
extern uint32_t TIMER_VALUES_PWM_BOOST[TIMER_BOOST_ARRAY_SIZE] =  {50, 100, 120, 150, 200, 300, 450, 600, 800, 1000, 1250, 1500, 2000, 2500, 3000, 4000, 5000, 7500, 10000, 20000, 100000};


/**
 * @brief Handler for timer events.
 */
void timer_pwm_boost_event_handler(nrf_timer_event_t event_type, void* p_context);
void timer_pwm_boost_ramp(uint32_t final_timer_boost_value, uint8_t delay);
void timer_pwm_boost_init(void);
void pwm_boost_activate(void);

/** @} */
#endif /* HM01B0_PWM_BOOST_H_ */