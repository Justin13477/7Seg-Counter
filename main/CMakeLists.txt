idf_component_register(
    SRCS "app.c" "main.c" "dcmotor.c" "7seg.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES freertos log esp_driver_gpio esp_driver_ledc
)
