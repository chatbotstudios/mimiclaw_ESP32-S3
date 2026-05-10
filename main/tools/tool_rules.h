#ifndef TOOL_RULES_H
#define TOOL_RULES_H

#include "esp_err.h"
#include <stddef.h>

/**
 * @brief Execute the local rule management tool.
 * 
 * input_json format:
 * {
 *   "action": "add"|"list"|"remove",
 *   "name": "string", (for add)
 *   "src": "temp"|"hum"|"batt", (for add)
 *   "cond": "gt"|"lt"|"eq"|"change", (for add)
 *   "threshold": number, (for add)
 *   "rule_action": "cli_command", (for add, e.g. "color red")
 *   "id": "R_1" (for remove)
 * }
 */
esp_err_t tool_rules_execute(const char *input_json, char *output, size_t output_size);

#endif // TOOL_RULES_H
