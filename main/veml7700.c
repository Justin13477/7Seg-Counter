#include "veml7700.h"

i2c_master_bus_handle_t veml7700_master_bus_handle;
i2c_master_dev_handle_t veml7700_dev_handle;
float lux;

esp_err_t veml7700_init(i2c_port_t i2c_num)
{
    veml7700_master_bus_handle = get_i2c_master_bus_handle(i2c_num); // Get the I2C master bus handle for the specified I2C port

    i2c_device_config_t veml7700_dev_conf = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7, // 7-bit address length
        .device_address = 0x10, // SHT41 I2C addy
        .scl_speed_hz = 100000, // SCL clock speed
        .scl_wait_us = 0, // No wait time for SCL
        .flags = { .disable_ack_check = 0 } // Enable ACK check
    };
    uint16_t veml_conf_byte = (VEML7700_G_1x | VEML7700_IT_100MS);
    
    esp_err_t err = i2c_master_bus_add_device(veml7700_master_bus_handle, &veml7700_dev_conf, &veml7700_dev_handle); // Create a new I2C device for the VEML7700 sensor
    if(err != ESP_OK)
    {
        ESP_LOGE("veml7700_init", "i2c add device failed");
        return ESP_FAIL;
    }

    return veml7700_write_register(veml7700_dev_handle, VEML7700_CONF_CMD, veml_conf_byte);
}

esp_err_t veml7700_write_register(i2c_master_dev_handle_t veml7700_dev_handle, uint8_t reg, uint16_t value)
{
    uint8_t data[3];

    data[0] = reg;
    data[1] = value & 0xFF;
    data[2] = (value >> 8) & 0xFF;

    return i2c_master_transmit(veml7700_dev_handle, data, sizeof(data), -1);
}

esp_err_t veml7700_read_register(i2c_master_dev_handle_t veml7700_dev_handle, uint8_t reg, uint8_t *value)
{
    if(value == NULL)
        {
            ESP_LOGE("veml7700_read_register", "value references to null pointer");
            return ESP_ERR_INVALID_ARG;
        }

    uint8_t data[2];
    esp_err_t err = i2c_master_transmit_receive(veml7700_dev_handle, &reg, 1, data, sizeof(data), -1);
    if(err != ESP_OK)
        {
            ESP_LOGE("veml7700_read_register", "Error during transmit and recieve");
            return ESP_FAIL;
        }

    *value = (uint16_t)data[0] | ((uint16_t)data[1] >> 8);

    return ESP_OK;
}

esp_err_t veml7700_read_lux(i2c_master_dev_handle_t veml7700_dev_handle, float *lux)
{
    uint8_t data[2];
    esp_err_t err = veml7700_read_register(veml7700_dev_handle, VEML7700_RAW_MEASUREMENT_CMD, data);
    if(err != ESP_OK)
    {
        ESP_LOGE("veml7700_read_lux", "Failed to read raw measurement");
        return ESP_FAIL;
    }

    uint16_t raw = (uint16_t)data[0] | ((uint16_t)data[1] << 8);

    *lux = veml7700_raw_to_lux(raw, VEML7700_RAW_CONVERSION);
    return ESP_OK;
}


float veml7700_raw_to_lux(uint16_t raw, float conversion)
{
    return (float)raw * conversion;
}

void veml7700_task(void *args)
{
    while(1)
    {
        esp_err_t err = veml7700_read_lux(veml7700_dev_handle, &lux);
        if(err != ESP_OK)
            ESP_LOGE("veml7700_task", "veml7700 failed to read");

        ESP_LOGI("VEML7700", "LUX: %.2f", lux);
        vTaskDelay(pdMS_TO_TICKS(5000)); // 5 second delay
    }
}