#pragma once

#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "esp_log.h"
#include "i2c.h"

// 0x10 Bus Address
#define VEML7700_CONF_CMD 0x00
#define VEML7700_RAW_MEASUREMENT_CMD 0x04

#define VEML7700_IT_100MS 0x0000
#define VEML7700_IT_200MS 0x0200
#define VEML7700_IT_400MS 0x0400
#define VEML7700_IT_800MS 0x0600
#define VEML7700_IT_50MS  0x0800
#define VEML7700_IT_25MS  0x0A00

#define VEML7700_G_2x     0x0000
#define VEML7700_G_1x     0x0800
#define VEML7700_G_14x    0x1000
#define VEML7700_G_12x    0x1800

#define VEML7700_RAW_CONVERSION 0.0576 // IT 100 MS X 1x GAIN

// IT x MS table for raw LUX conversion     (float)raw * x
// IT(MS)  GAIN 2   GAIN 1   GAIN 1/4   GAIN 1/2
// 800     0.0036   0.0072   0.0288     0.0576
// 400     0.0072   0.0144   0.0576     0.1152
// 200     0.0144   0.0288   0.1152     0.2304
// 100     0.0288   0.0576   0.2304     0.4608
// 50      0.0576   0.1152   0.4608     0.9216
// 25      0.1152   0.2304   0.9216     1.8432

esp_err_t veml7700_init(i2c_port_t i2c_num);
esp_err_t veml7700_write_register(i2c_master_dev_handle_t veml7700_dev_handle, uint8_t reg, uint16_t value);
esp_err_t veml7700_read_register(i2c_master_dev_handle_t veml7700_dev_handle, uint8_t reg, uint8_t *value);

esp_err_t veml7700_read_lux(i2c_master_dev_handle_t veml7700_dev_handle, float *lux);

float veml7700_raw_to_lux(uint16_t raw, float conversion);

void veml7700_task(void *args);