idf_component_register(
    SRCS "veml7700.c" "sht41.c" "i2c.c" "app.c" "main.c" "dcmotor.c" "7seg.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES freertos log esp_driver_gpio esp_driver_i2c esp_driver_ledc
)
