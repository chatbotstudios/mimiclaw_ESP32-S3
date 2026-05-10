#ifndef TOOL_AGENT_H
#define TOOL_AGENT_H

#include "esp_err.h"
#include <stddef.h>

/**
 * @brief Manage AI agent diagnostics and internal state
 * 
 * @param input_json JSON with "action" (metrics|audit|stack|uptime)
 * @param output Response JSON
 * @param output_size Size of response buffer
 * @return esp_err_t 
 */
esp_err_t tool_agent_execute(const char *input_json, char *output, size_t output_size);

#endif // TOOL_AGENT_H
