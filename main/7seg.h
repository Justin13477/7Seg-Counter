#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

esp_err_t seven_seg_init(void);
uint8_t get_7seg_encoding(uint8_t digit, uint8_t dp);
esp_err_t send_digit(uint8_t encoding, uint8_t digit_position);
esp_err_t send_number(uint8_t number, uint8_t digit_position, uint8_t dp);
uint8_t count_digits(uint16_t number);

void set_display_number(uint16_t number, uint8_t dp);
void display_task(void *arg);