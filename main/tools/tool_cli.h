#ifndef TOOL_CLI_H
#define TOOL_CLI_H

#include "esp_err.h"
#include <stddef.h>

/**
 * @brief Execute a CLI command and capture its output
 *
 * @param input_json JSON with "command" string
 * @param output Buffer to store console output
 * @param output_size Size of output buffer
 * @return esp_err_t
 */
esp_err_t tool_cli_execute(const char *input_json, char *output,
                           size_t output_size);

#endif
