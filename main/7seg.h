#pragma once

#include <stdint.h>
#include "esp_err.h"

esp_err_t seven_seg_init(void);
uint8_t get_7seg_encoding(uint8_t digit, uint8_t dp);
esp_err_t send_digit(uint8_t encoding, uint8_t digit_position);
esp_err_t send_number(uint8_t number, uint8_t digit_position, uint8_t dp);