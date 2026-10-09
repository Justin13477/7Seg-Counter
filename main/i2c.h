#pragma once

#include "driver/i2c_master.h"
#include "esp_err.h"

esp_err_t i2c_master_init(i2c_port_t i2c_num, gpio_num_t sda_gpio, gpio_num_t scl_gpio);

i2c_master_bus_handle_t get_i2c_master_bus_handle(i2c_port_t i2c_num);
