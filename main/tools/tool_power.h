#ifndef TOOL_POWER_H
#define TOOL_POWER_H

#include "esp_err.h"
#include <stddef.h>

/**
 * @brief Manage system power, battery, and modes
 * 
 * @param input_json JSON with "action" (status|set_mode|hibernate) and parameters
 * @param output Response JSON
 * @param output_size Size of response buffer
 * @return esp_err_t 
 */
esp_err_t tool_power_execute(const char *input_json, char *output, size_t output_size);

#endif // TOOL_POWER_H
