#include "7seg.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define Dig0 0b11111100
#define Dig1 0b01100000
#define Dig2 0b11011010
#define Dig3 0b11110010
#define Dig4 0b01100110
#define Dig5 0b10110110
#define Dig6 0b10111110
#define Dig7 0b11100000
#define Dig8 0b11111110
#define Dig9 0b11110110

gpio_config_t io_7seg_conf = {
    .pin_bit_mask =         // Configure GPIO pins for 4-digit 7-segment display
    1ULL << GPIO_NUM_14 |   // A:  7 Seg - Pin 11: ESP32 - 14
    1ULL << GPIO_NUM_13 |   // B:  7 Seg - Pin 7:  ESP32 - 13
    1ULL << GPIO_NUM_12 |   // C:  7 Seg - Pin 4:  ESP32 - 12
    1ULL << GPIO_NUM_11 |   // D:  7 Seg - Pin 2:  ESP32 - 11
    1ULL << GPIO_NUM_10 |   // E:  7 Seg - Pin 1:  ESP32 - 10
    1ULL << GPIO_NUM_9 |   // F:  7 Seg - Pin 10: ESP32 - 9
    1ULL << GPIO_NUM_46 |    // G:  7 Seg - Pin 5:  ESP32 - 46
    1ULL << GPIO_NUM_3|    // DP: 7 Seg - Pin 10: ESP32 - 3
    1ULL << GPIO_NUM_8 |    // D1: 7 Seg - Pin 12: ESP32 - 8
    1ULL << GPIO_NUM_18 |    // D2: 7 Seg - Pin 9:  ESP32 - 18
    1ULL << GPIO_NUM_17 |    // D3: 7 Seg - Pin 8:  ESP32 - 17
    1ULL << GPIO_NUM_16,     // D4: 7 Seg - Pin 6:  ESP32 - 16
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE, 
    .pull_down_en = GPIO_PULLDOWN_ENABLE,
    .intr_type = GPIO_INTR_DISABLE
};

uint8_t digit_to_7seg[10] = {
    Dig0, // 0
    Dig1, // 1
    Dig2, // 2
    Dig3, // 3
    Dig4, // 4
    Dig5, // 5
    Dig6, // 6
    Dig7, // 7
    Dig8, // 8
    Dig9  // 9
};

gpio_num_t digit_pins[7] = {
    GPIO_NUM_14,  // A
    GPIO_NUM_13,  // B
    GPIO_NUM_12,  // C
    GPIO_NUM_11,  // D
    GPIO_NUM_10,  // E
    GPIO_NUM_9,   // F
    GPIO_NUM_46   // G   
};

uint8_t get_7seg_encoding(uint8_t digit, uint8_t dp)
{
    // maps the digit to its corresponding 7-segment encoding, and sets the DP bit if needed
    uint8_t encoding = 0;
    if (digit < 10) {
        ESP_LOGI("7SEG", "Digit: %d, Encoding: 0x%02x", digit, digit_to_7seg[digit]);
        encoding = digit_to_7seg[digit];
    } else { 
        ESP_LOGE("7SEG", "Invalid digit: %d", digit);
        return 0; // invalid digit
    }
    
    if(dp) {
        encoding |= 0x01; // Set the DP bit
    }
    return encoding;
}


esp_err_t send_digit(uint8_t encoding, uint8_t digit_position)
{
    if(digit_position < 1 || digit_position > 4)
    {
        ESP_LOGE("7SEG", "Invalid digit position");
        return ESP_ERR_INVALID_ARG;
    }
    // Implementation for sending the encoding to the 7-segment display based on the digit position
    switch(digit_position)
    {
        case 1: 
            gpio_set_level(GPIO_NUM_8, 1); // Activate D1;
            gpio_set_level(GPIO_NUM_18, 0); // Deactivate D2;
            gpio_set_level(GPIO_NUM_17, 0); // Deactivate D3;
            gpio_set_level(GPIO_NUM_16, 0); // Deactivate D4;
            break;
        case 2:
            gpio_set_level(GPIO_NUM_8, 0); // Deactivate D1;
            gpio_set_level(GPIO_NUM_18, 1); // Activate D2;
            gpio_set_level(GPIO_NUM_17, 0); // Deactivate D3;
            gpio_set_level(GPIO_NUM_16, 0); // Deactivate D4; 
            break;
        case 3:
            gpio_set_level(GPIO_NUM_8, 0); // Deactivate D1;
            gpio_set_level(GPIO_NUM_18, 0); // Deactivate D2;
            gpio_set_level(GPIO_NUM_17, 1); // Activate D3;
            gpio_set_level(GPIO_NUM_16, 0); // Deactivate D4; 
            break;
        case 4:
            gpio_set_level(GPIO_NUM_8, 0); // Deactivate D1;
            gpio_set_level(GPIO_NUM_18, 0); // Deactivate D2;
            gpio_set_level(GPIO_NUM_17, 0); // Deactivate D3;
            gpio_set_level(GPIO_NUM_16, 1); // Activate D4; 
            break;
    }

    // loops through the 7 segments and sets the corresponding GPIO levels based on the encoding
    for (uint8_t i = 0; i < 7; i++)
    {
        // This display's segment inputs are active-low: 0 in the encoding means ON.
        uint8_t bit_value = ((encoding >> (7 - i)) & 0x01) ^ 0x01;
        gpio_set_level(digit_pins[i], bit_value);
    }
    gpio_set_level(GPIO_NUM_3, (encoding & 0x01) ^ 0x01); // Set DP pin based on encoding
    return ESP_OK;
}

// wrapper to send a number to the 7-segment display, given a digit position (1-4)
esp_err_t send_number(uint8_t number, uint8_t digit_position, uint8_t dp)
{
    if (number > 9) {
        ESP_LOGE("7SEG", "Invalid number: %d", number);
        return ESP_ERR_INVALID_ARG;
    }
    uint8_t encoding = get_7seg_encoding(number, dp);
    return send_digit(encoding, digit_position);
}

uint8_t count_digits(uint16_t number)
{
    uint8_t digit_count = 0;
    do {
        number /= 10;
        digit_count++;
    } while (number > 0);

    return (uint8_t)digit_count;
    ESP_LOGI("7SEG", "Number of digits: %d", digit_count);
}

// configures the GPIO pins for the 7-segment display
esp_err_t seven_seg_init(void)
{
    xTaskCreate(display_task, "display_task", 2048, NULL, 10, NULL); // Create a FreeRTOS task to handle display multiplexing
    return gpio_config(&io_7seg_conf);
}

uint16_t number_d;
uint8_t dp_d;

void set_display_number(uint16_t num, uint8_t decimal)
{
    number_d = num;
    dp_d = decimal;
}

void display_task(void *arg)
{
    while(1)
    {
        if (number_d > 9999) {
        ESP_LOGE("7SEG", "Invalid number: %d", number_d);
    }
    uint8_t digit_count = count_digits(number_d);
    ESP_LOGI("7SEG", "Sending number: %d, starting at position: %d", number_d, digit_count);
    

    uint16_t digit = number_d;
    for (uint8_t i = 0; i < digit_count; i++) {
        
        uint8_t current_digit = digit % 10;
        send_number(current_digit, digit_count - i, dp_d);
        vTaskDelay(pdMS_TO_TICKS(4));

        digit /= 10;
     
    }

        
    }
}