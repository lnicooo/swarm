#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

#define GPIO_PWM0A_OUT 25  //Set GPIO 19 as PWM0A
#define GPIO_PWM0B_OUT 26  //Set GPIO 18 as PWM0B
#define GPIO_PWM1A_OUT 27   //Set GPIO 17 as PWM1A
#define GPIO_PWM1B_OUT 14 //Set GPIO 16 as PWM1B


void mcpwm_example_gpio_initialize();
void brushed_motor_forward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num , float duty_cycle);
void brushed_motor_backward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num , float duty_cycle);
void brushed_motor_stop(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num);