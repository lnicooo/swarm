#include <stdio.h>
#include "driver/ledc.h"

#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_GPIO       (33)
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0
#define LEDC_HS_CH1_GPIO       (12)
#define LEDC_HS_CH1_CHANNEL    LEDC_CHANNEL_1
#define LEDC_HS_CH2_GPIO       (32)
#define LEDC_HS_CH2_CHANNEL    LEDC_CHANNEL_2


#define LEDC_HS_CH3_GPIO       (4)
#define LEDC_HS_CH3_CHANNEL    LEDC_CHANNEL_3
#define LEDC_HS_CH4_GPIO       (17)
#define LEDC_HS_CH4_CHANNEL    LEDC_CHANNEL_4
#define LEDC_HS_CH5_GPIO       (16)
#define LEDC_HS_CH5_CHANNEL    LEDC_CHANNEL_5


#define LEDC_TEST_CH_NUM       (3)
#define LEDC_TEST_DUTY         (255)
#define LEDC_TEST_FADE_TIME    (3000)

extern ledc_channel_config_t led_sensor[LEDC_TEST_CH_NUM];
extern ledc_channel_config_t led_status[LEDC_TEST_CH_NUM];

void led_init(ledc_channel_config_t *ledc_channel);
void set_led_color(ledc_channel_config_t *ledc_channel, uint32_t color_code);
