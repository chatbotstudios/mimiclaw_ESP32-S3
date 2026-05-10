#ifndef TOOL_LED_H
#define TOOL_LED_H

#include "esp_err.h"
#include <stddef.h>

esp_err_t tool_led_control_execute(const char *input_json, char *output, size_t output_size);

#endif // TOOL_LED_H
