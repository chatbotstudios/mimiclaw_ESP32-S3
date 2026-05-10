#ifndef TOOL_NETWORK_H
#define TOOL_NETWORK_H

#include "esp_err.h"
#include <stddef.h>

/**
 * @brief Manage network diagnostics and sync
 * 
 * @param input_json JSON with "action" (ping|scan|info|sync) and parameters
 * @param output Response JSON
 * @param output_size Size of response buffer
 * @return esp_err_t 
 */
esp_err_t tool_network_execute(const char *input_json, char *output, size_t output_size);

#endif // TOOL_NETWORK_H
