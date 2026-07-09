#pragma once

#include "esp_err.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t tool_touch_execute(const char *input_json, char *output, size_t output_size);

#ifdef __cplusplus
}
#endif
