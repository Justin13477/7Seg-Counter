#pragma once

#include "7seg.h"
#include "sht41.h"
#include "i2c.h"
#include "veml7700.h"

#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

void init_button_gpio();
esp_err_t init_i2c();
esp_err_t init_sht41();
esp_err_t init_veml7700();
void init_app();

void app_run();

void button_task(void *arg);

void increment_counter_task(void *arg);