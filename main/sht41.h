#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "i2c.h"

#include <stdint.h>
#include <stddef.h>

#define SHT41_TEMP_MEASURE_CMD 0xFD // command for high precision temperature measurement

esp_err_t sht41_init(i2c_port_t i2c_num);
esp_err_t sht41_read_data();

uint8_t sht41_calculate_crc(uint8_t *data, size_t len); // Function to calculate CRC for data integrity check

void sht41_convert(uint16_t temp_raw, uint16_t hum_raw, float *temp_c, float *temp_f, float *hum_percent);


void sht41_task(void *arg); // RTOS task to read data



