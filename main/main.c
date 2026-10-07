#include "dcmotor.h"
#include "7seg.h"
#include "app.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_log.h"




void app_main(void)
{

    if (seven_seg_init() != ESP_OK) {
        return;
    }

    if (dcmotor_init() != ESP_OK) {
        return;
    }

    if(seven_seg_init() != ESP_OK) {
        return;
    }

    app_run();
    //set_display_number(4965, 0); // Set the number to be displayed on the 7-segment display

    
    //dcmotor_set_duty(128);
}
