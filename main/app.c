#include "app.h"
#include "esp_log.h"


esp_err_t init_i2c()
{
    if(i2c_master_init(I2C_NUM_0, GPIO_NUM_7, GPIO_NUM_6) != ESP_OK) // init i2c master
    {
        ESP_LOGE("I2C", "Failed to init i2c master");
        return ESP_FAIL;
    }
    return ESP_OK;
}
esp_err_t init_sht41()
{
    if(sht41_init(I2C_NUM_0) != ESP_OK) // Initialize the SHT41 sensor on I2C port 0
    {
        ESP_LOGE("SHT41", "Failed to initialize SHT41 sensor");
        return ESP_FAIL;
        
    }
    return ESP_OK;
}

esp_err_t init_veml7700()
{
    if(veml7700_init(I2C_NUM_0) != ESP_OK)
    {
        ESP_LOGE("init_veml7700", "Failed to init veml7700");
        return ESP_FAIL;
    }
    return ESP_OK;
}


void app_run()
{
    
    init_button_gpio(); // Initialize the button GPIO and ISR
    init_i2c(); // Initialize the I2C master
    init_veml7700();
    xTaskCreate(veml7700_task, "veml7700 task", 2048, NULL, 10, NULL);
    //init_sht41(); // Initialize the SHT41 sensor
    //xTaskCreate(button_task, "button_task", 2048, NULL, 10, NULL); // Create a FreeRTOS task to handle button press events
    //xTaskCreate(increment_counter_task, "increment_counter_task", 2048, NULL, 10, NULL); // Create a FreeRTOS task to handle counter increment on button press
    //xTaskCreate(sht41_task, "sht41_task", 2048, NULL, 10, NULL);

    //set_display_number(5891, 0); // Initialize the 7-segment display with the initial counter value

    
}