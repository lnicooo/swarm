/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_defs.h"

#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"

#include "esp_event_loop.h"
#include "esp_system.h"
#include "esp_event.h"

#include "mpu9250.h"
#include "color_sensor.h"
#include "led.h"
#include "esp_beacon_api.h"
#include "motor_driver.h"

static const char* DEMO_TAG = "ROBOT_BEACON_DEV";

uint32_t status;
struct MPU_h* mpu;

typedef struct{
        float x;
        float y;
        float z;
}accel_t;


///Declare static functions
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

static esp_ble_scan_params_t ble_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x50,
    .scan_window            = 0x30,
    .scan_duplicate         = BLE_SCAN_DUPLICATE_DISABLE
};

static esp_ble_adv_params_t ble_adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_NONCONN_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    esp_err_t err;

    switch (event) {

    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: {

        //the unit of the duration is second, 0 means scan permanently
        uint32_t duration = 0;
        esp_ble_gap_start_scanning(duration);

        break;
    }
    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        //scan start complete event to indicate scan start successfully or failed
        if ((err = param->scan_start_cmpl.status) != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(DEMO_TAG, "Scan start failed: %s", esp_err_to_name(err));
        }
        break;
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        //adv start complete event to indicate adv start successfully or failed
        if ((err = param->adv_start_cmpl.status) != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(DEMO_TAG, "Adv start failed: %s", esp_err_to_name(err));
        }
            break;
        case ESP_GAP_BLE_SCAN_RESULT_EVT: {
            esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
            switch (scan_result->scan_rst.search_evt) {
            case ESP_GAP_SEARCH_INQ_RES_EVT:
            /* Search for BLE Robot Beacon Packet */
            {   

                esp_ble_beacon_t *beacon_data = (esp_ble_beacon_t*)(scan_result->scan_rst.ble_adv);
                if(esp_ble_check_beacon(beacon_data)){

                    ESP_LOGI(DEMO_TAG, "----------Robot Beacon Found----------");
                    esp_log_buffer_hex("BEACON_DEMO: Device address:", scan_result->scan_rst.bda, ESP_BD_ADDR_LEN );
                    

                    ESP_LOGI(DEMO_TAG, "BEACON_DEMO: Robot ID:%d", beacon_data->robot_id);
                    esp_log_buffer_hex("BEACON_DEMO: UUID:", beacon_data->uuid, UUID_LEN );
                    
                    ESP_LOGI(DEMO_TAG, "Position %d",beacon_data->position);
                    ESP_LOGI(DEMO_TAG, "Status %x",beacon_data->robot_status);

                    ESP_LOGI(DEMO_TAG, "Measured power (RSSI at a 1m distance):%d dbm", beacon_data->measured_power);
                    ESP_LOGI(DEMO_TAG, "RSSI of packet:%d dbm", scan_result->scan_rst.rssi);
                    float distance = pow(10,((-59-(scan_result->scan_rst.rssi))/20));
                    ESP_LOGI(DEMO_TAG, "distance:%0.2f m",distance);

                    //status = beacon_data->robot_status;

                }
                else{

                    ESP_LOGE(DEMO_TAG, "Not robot beaon");
                    
                }
                

            }
            
            break;
        default:
            break;
        }
        break;
    }

    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        if ((err = param->scan_stop_cmpl.status) != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(DEMO_TAG, "Scan stop failed: %s", esp_err_to_name(err));
        }
        else {
            //ESP_LOGI(DEMO_TAG, "Stop scan successfully");
        }
        break;

    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if ((err = param->adv_stop_cmpl.status) != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(DEMO_TAG, "Adv stop failed: %s", esp_err_to_name(err));
        }
        else {
            ESP_LOGI(DEMO_TAG, "Stop adv successfully");
        }
        break;

    default:
        break;
    }
}

void create_beacon_task()
{
    while(1){
        esp_ble_gap_stop_advertising();
        uint32_t color = get_color();
        //ESP_LOGI(DEMO_TAG, "Read color:%d",color);  
        //esp_err_t status = esp_ble_config_beacon_data (&robot_adv_beacon, 0, color);
        robot_adv_beacon.robot_status = color;
        esp_ble_gap_config_adv_data_raw((uint8_t*)&robot_adv_beacon, sizeof(robot_adv_beacon));
        

        esp_ble_gap_start_advertising(&ble_adv_params);
        vTaskDelay(pdMS_TO_TICKS(1000));
    
    }
    
}

void move_task()
{   
    accel_t pA={};
    accel_t dA={};
    accel_t accel={};
    accel_t gyro ={};
    imu_sensor_data_t imu_d ={};

    int step=0;
    float dt;

    float yaw=0;

    while (true) {

        mpu9250_read_gyro_accel(&imu_d);

        accel.x = imu_d.accel[0]/MPU9250_ACCE_SENS_2;
        accel.y = imu_d.accel[1]/MPU9250_ACCE_SENS_2;

        gyro.z = imu_d.gyro[2]/MPU9250_GYRO_SENS_500;
        
        
        if(step>0){

            dA.x = accel.x - pA.x;
            dA.y = accel.y - pA.y;
            
        }

        pA.x = accel.x;
        pA.y = accel.y;
        step = 1;

        yaw += gyro.z * (1.0/250.0);

        dt = fabs(dA.x) + fabs(dA.y);
        
        printf("%f\n",yaw);
        //ESP_LOGI(DEMO_TAG, "Dt: %f\n yaw: %f\n", dt, yaw);
        //printf("accel: [%+6.2f %+6.2f %+6.2f ] (G)\n", dA.x, dA.y, gyro.z);

        /*
        if(dt>4500){
        
            brushed_motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_0, 30.0);
            brushed_motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_1, 50.0);
            vTaskDelay(2000 / portTICK_RATE_MS);
            brushed_motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_0); 
            brushed_motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_1);
            vTaskDelay(500 / portTICK_RATE_MS);
            dt=0;
        }
        else{
            
            brushed_motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, 80.0);
            brushed_motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_1, 80.0);
        }
        */
        
        vTaskDelay(100 / portTICK_RATE_MS);
    }
}

void status_color_task()
{
    while(1){

        set_led_color( led_status, 0x0000ff);
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void ble_beacon_init(void)
{   
    esp_err_t status;

    
    esp_bluedroid_init();
    esp_bluedroid_enable();

    ESP_LOGI(DEMO_TAG, "register callback");

    //register the scan callback function to the gap module
    if ((status = esp_ble_gap_register_callback(esp_gap_cb)) != ESP_OK) {
        ESP_LOGE(DEMO_TAG, "gap register error: %s", esp_err_to_name(status));
        return;
    }
}

void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());
     
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);

    mpu9250_init(MPU9250_ACCE_SENS_2, MPU9250_GYRO_SENS_500);
    color_sensor_init();
    led_init(led_status);
    ble_beacon_init();
    mcpwm_example_gpio_initialize();
    //esp_ble_gap_config_adv_data_raw((uint8_t*)&robot_adv_beacon, sizeof(robot_adv_beacon));

    //esp_ble_gap_set_scan_params(&ble_scan_params);

    //xTaskCreate(&create_beacon_task, "create_beacon_task", 2048, NULL, 5, NULL);
    //xTaskCreate(&status_color_task, "status_color_task", 2048, NULL, 5, NULL);
    xTaskCreate(&move_task, "move_task", 2048, NULL, 5, NULL);
}

