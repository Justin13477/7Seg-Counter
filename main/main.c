#include "dcmotor.h"
#include "7seg.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_log.h"


void loop_nums()
{
    for (uint8_t i = 0; i < 10; i++)
    {
        for(uint8_t j = 1; j <= 4; j++)
        {
            vTaskDelay(pdMS_TO_TICKS(1000)); // Wait for 1 second before trying again
            if(send_number(i, j, 0) != ESP_OK)
            {
                ESP_LOGE("7SEG", "Failed to send number %d to digit position %d", i, j);
                return;
            }
        }
    }
}

void app_main(void)
{
    if (seven_seg_init() != ESP_OK) {
        return;
    }

    if (dcmotor_init() != ESP_OK) {
        return;
    }

    loop_nums();

    dcmotor_set_duty(128);
}
