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

#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"

#define ROBOT_ID  0xEF	


typedef struct {
	uint8_t flags[3];
    uint8_t length;
    uint8_t type;
    uint16_t beacon_type;
    uint16_t robot_id;
    uint32_t robot_status;
    uint16_t position;
    int8_t measured_power;
}__attribute__((packed)) esp_ble_beacon_t;

extern volatile esp_ble_beacon_t robot_adv_beacon;

esp_err_t esp_ble_config_beacon_data (esp_ble_beacon_t *data_beacon, uint16_t position, uint32_t robot_status);
