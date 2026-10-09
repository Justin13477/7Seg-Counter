#include "sht41.h"

i2c_master_bus_handle_t master_bus_handle;
i2c_master_dev_handle_t sht41_dev_handle;

uint8_t sht41_data[6]; // buffer to hold sht41 data
uint16_t temperature_raw;
uint16_t humidity_raw;
float temperature_c;
float temperature_f;
float humidity_percent;


uint8_t command = SHT41_TEMP_MEASURE_CMD; // command to send to sht41 sensor

esp_err_t sht41_init(i2c_port_t i2c_num) {
    master_bus_handle = get_i2c_master_bus_handle(i2c_num); // Get the I2C master bus handle for the specified I2C port

    i2c_device_config_t sht41_dev_conf = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7, // 7-bit address length
        .device_address = 0x44, // SHT41 I2C addy
        .scl_speed_hz = 100000, // SCL clock speed
        .scl_wait_us = 0, // No wait time for SCL
        .flags = { .disable_ack_check = 0 } // Enable ACK check
    };
    return i2c_master_bus_add_device(master_bus_handle, &sht41_dev_conf, &sht41_dev_handle); // Create a new I2C device for the SHT41 sensor

}

uint8_t sht41_calculate_crc(uint8_t *data, size_t len)
{
    uint8_t crc = 0xFF; // Initialize CRC byte to 0xFF
    for(size_t i = 0; i < len; i++)
    {
        crc ^= data[i]; // XOR the current byte to the CRC
        for(uint8_t bit = 0; bit < 8; bit++)
        {
            if(crc & 0x80) // checks if msb is set
            {
                crc = (crc << 1) ^ 0x31; // shift left and XOR with polynomial 0x31
            } 
            else 
            {
                crc <<= 1; // shift left
            }
        }
    }
    return crc;
}

esp_err_t sht41_read_data() {
    i2c_master_transmit(sht41_dev_handle, &command, sizeof(command), -1); // send command
    vTaskDelay(pdMS_TO_TICKS(10)); // wait for measurement
    i2c_master_receive(sht41_dev_handle, sht41_data, 6, -1); // read 6 bytes back

    if(sht41_calculate_crc(sht41_data, 2) != sht41_data[2]) 
    {
        ESP_LOGE("SHT41", "Temperature CRC check failed");
        //return ESP_ERR_INVALID_CRC; // CRC check failed
    }
    temperature_raw = (sht41_data[0] << 8 | sht41_data[1]);

    if(sht41_calculate_crc(&sht41_data[3], 2) != sht41_data[5])
    {
        ESP_LOGE("SHT41", "Humidity CRC check failed");
        //return ESP_ERR_INVALID_CRC; // CRC check failed
    }
    humidity_raw = (sht41_data[3] << 8 | sht41_data[4]);

    return ESP_OK; // data read successful
}

void sht41_convert(uint16_t temp_raw, uint16_t hum_raw, float *temp_c, float *temp_f, float *hum_percent) {
    *temp_c = -45 + 175.0f * ((float)temp_raw / 65535.0f); // convert raw temp to celsius
    *temp_f = (*temp_c * 9.0f / 5.0f) + 32.0f; // convert c to f
    *hum_percent = 100.0f * ((float)hum_raw / 65535.0f); // convert raw humidity to percentage
}

void sht41_task(void *arg) 
{
    while(1)
    {
        if(sht41_read_data() != ESP_OK)
        {
            ESP_LOGE("SHT41", "Failed to read data from SHT41 sensor");
        } 
        else 
        {
            sht41_convert(temperature_raw, humidity_raw, &temperature_c, &temperature_f, &humidity_percent);
            ESP_LOGI("SHT41", "Temperature: %.2f C / %.2f F, Humidity: %.2f%%", temperature_c, temperature_f, humidity_percent);
        }
        vTaskDelay(pdMS_TO_TICKS(5000)); 
    }
}