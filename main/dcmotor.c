#include "dcmotor.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#define MOTOR_PWM_GPIO GPIO_NUM_7

ledc_timer_config_t timer_conf = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .timer_num = LEDC_TIMER_0,
    .duty_resolution = LEDC_TIMER_8_BIT, 
    .freq_hz = 1000,
    .clk_cfg = LEDC_AUTO_CLK

}; 

ledc_channel_config_t channel_conf = {
    .gpio_num = MOTOR_PWM_GPIO,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_0, 
    .intr_type = LEDC_INTR_DISABLE, 
    .timer_sel = LEDC_TIMER_0,
    .duty = 0,
    .hpoint = 0,
}; 

gpio_config_t pwm_conf = {
    .pin_bit_mask = 1ULL << MOTOR_PWM_GPIO,
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE, 
    .pull_down_en = GPIO_PULLDOWN_ENABLE,
    .intr_type = GPIO_INTR_DISABLE
}; 

esp_err_t dcmotor_init(void)
{
    esp_err_t result = ledc_timer_config(&timer_conf);
    if (result != ESP_OK) {
        return result;
    }

    result = ledc_channel_config(&channel_conf);
    if (result != ESP_OK) {
        return result;
    }

    return gpio_config(&pwm_conf);
}

esp_err_t dcmotor_set_duty(uint32_t duty)
{
    esp_err_t result = ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    if (result != ESP_OK) {
        return result;
    }

    return ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}