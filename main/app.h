#pragma once

#include "7seg.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

void init_button_gpio();

void app_run();

void button_task(void *arg);

void increment_counter_task(void *arg);