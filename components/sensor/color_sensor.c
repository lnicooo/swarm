#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "color_sensor.h"


uint16_t C[3];

static const uint16_t K[3] = {4095, 4158, 4159};
static const uint16_t W[3] = {1532, 1444, 2242};

void color_sensor_init()
{
    //Configure ADC
    
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(channel, atten);
    led_init(led_sensor);

    //Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
}

uint32_t get_color()
{
    

    uint32_t led_color = 0xff0000;

    uint32_t C_8b = 0;
    
    uint32_t adc_reading = 0;
    //Multisampling
    for (int rgb = 0; rgb<3; rgb++){

        if(led_color <= 0)
            led_color=0xff0000;

        set_led_color(led_sensor, led_color);

        led_color = led_color >> 8;

        vTaskDelay(pdMS_TO_TICKS(50));
        
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
   
            adc_reading += adc1_get_raw((adc1_channel_t)channel);
            
        }
        
        adc_reading /= NO_OF_SAMPLES;
        
        C[rgb] = 4095 * ((uint16_t)adc_reading - K[rgb]) / (W[rgb] - K[rgb]);

        C_8b |= (((C[rgb]* 256)/4096) & 0xff) << (16-(8*rgb));

        //printf("-----------\n%d %d\n C[%d] = %d\n",rgb, adc_reading, rgb, C[rgb]);
        
    }

    
     set_led_color(led_sensor, 0x000000);

    return C_8b;
    
}