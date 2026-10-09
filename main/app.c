#include "app.h"
#include "esp_log.h"


gpio_config_t io_button_conf = {
    .pin_bit_mask = 1ULL << GPIO_NUM_4, // Configure GPIO pin for button input
    .mode = GPIO_MODE_INPUT, 
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_ENABLE,
    .intr_type = GPIO_INTR_NEGEDGE // interrupt on falling edge
}; // gpio_config(&io_button_conf);

static QueueHandle_t button_event_queue; // Queue to hold button press events
static SemaphoreHandle_t button_semaphore; // Semaphore to signal button press events
uint16_t counter = 0;

void IRAM_ATTR button_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg; // Get the GPIO number from the argument

    BaseType_t xHigherPriorityTaskWoken = pdFALSE; // Variable to check if a higher priority task was woken

    xQueueSendFromISR(button_event_queue, &gpio_num, &xHigherPriorityTaskWoken); // Send the GPIO number to the queue from ISR

    if(xHigherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR(); // Yield to a higher priority task if needed
    }
}

void init_button_gpio()
{
    if(gpio_config(&io_button_conf) != ESP_OK)
    {
        ESP_LOGE("GPIO", "Failed to configure button GPIO");
        return;
    }
    if(gpio_install_isr_service(0) != ESP_OK)
    {
        ESP_LOGE("GPIO", "Failed to install ISR service");
        return;
    }
    if(gpio_isr_handler_add(GPIO_NUM_4, button_isr_handler, NULL) != ESP_OK) // Add ISR handler for the button GPIO
    {
        ESP_LOGE("GPIO", "Failed to add ISR handler");
        return;
    }

    button_event_queue = xQueueCreate(10, sizeof(uint32_t)); // Create a queue to hold button press events
    button_semaphore = xSemaphoreCreateBinary(); // Create a binary semaphore for button press events
}

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

void button_task(void *arg)
{
    uint32_t gpio_num;
    while(1)
    {
        if(xQueueReceive(button_event_queue, &gpio_num, portMAX_DELAY)) // Wait for a button press event
        {
            xSemaphoreGive(button_semaphore); // Signal the button press event
            ESP_LOGI("BUTTON", "Button pressed on GPIO %d", gpio_num); // Log the button press event
            // Handle the button press event here (e.g., toggle an LED, send a message, etc.)
        }
    }
}


void increment_counter_task(void *arg)
{
    while(1)
    {
        if(xSemaphoreTake(button_semaphore, portMAX_DELAY)) // Wait for the button press semaphore
        {
            // Increment the counter or perform any action you want on button press
            ESP_LOGI("COUNTER", "Button press detected, incrementing counter");
            // Add your counter increment logic here
            if(counter < 9999) {
                counter++;
            } else {
                counter = 0; // Reset counter if it exceeds 9
            }
            ESP_LOGI("COUNTER", "Current counter value: %d", counter);
            set_display_number(counter, 0); // Update the 7-segment display with the new counter value
        }
    }
}

void app_run()
{
    
    init_button_gpio(); // Initialize the button GPIO and ISR
    init_i2c(); // Initialize the I2C master
    init_sht41(); // Initialize the SHT41 sensor
    xTaskCreate(button_task, "button_task", 2048, NULL, 10, NULL); // Create a FreeRTOS task to handle button press events
    xTaskCreate(increment_counter_task, "increment_counter_task", 2048, NULL, 10, NULL); // Create a FreeRTOS task to handle counter increment on button press
    xTaskCreate(sht41_task, "sht41_task", 2048, NULL, 10, NULL);

    //set_display_number(5891, 0); // Initialize the 7-segment display with the initial counter value

    
}