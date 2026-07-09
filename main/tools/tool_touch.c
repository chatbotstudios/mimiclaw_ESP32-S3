#include "tool_touch.h"
#include <stdio.h>
#include <string.h>

esp_err_t tool_touch_execute(const char *input_json, char *output, size_t output_size) {
    snprintf(output, output_size, "{\"status\":\"error\",\"message\":\"Touch tool temporarily disabled while migrating to official BSP.\"}");
    return ESP_OK;
}
