#include "i2c.h"

i2c_master_bus_handle_t i2c_master_handle;

esp_err_t i2c_master_init(i2c_port_t i2c_num, gpio_num_t sda_gpio, gpio_num_t scl_gpio)
{
    i2c_master_bus_config_t conf = {
        .i2c_port = i2c_num,
        .sda_io_num = sda_gpio,
        .scl_io_num = scl_gpio,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7, 
        .flags.enable_internal_pullup = true
    };
    if(i2c_new_master_bus(&conf, &i2c_master_handle) != ESP_OK) {
        return ESP_FAIL;
    }
    return ESP_OK;
}


i2c_master_bus_handle_t get_i2c_master_bus_handle(i2c_port_t i2c_num)
{
    return i2c_master_handle;
}