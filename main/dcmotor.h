#pragma once

#include <stdint.h>
#include "esp_err.h"


esp_err_t dcmotor_init(void);
esp_err_t dcmotor_set_duty(uint32_t duty);