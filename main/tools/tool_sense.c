#include "tools/tool_sense.h"
#include "hardware/shtc3.h"
#include "cJSON.h"
#include <stdio.h>
#include <string.h>
#include "esp_log.h"

esp_err_t tool_sense_execute(const char *input_json, char *output, size_t output_size) {
    cJSON *root = cJSON_Parse(input_json);
    cJSON *action_item = cJSON_GetObjectItem(root, "action");
    const char *action = cJSON_IsString(action_item) ? action_item->valuestring : "read";

    if (strcmp(action, "read_raw") == 0) {
        shtc3_raw_data_t raw;
        if (shtc3_read_raw(&raw) == ESP_OK) {
            snprintf(output, output_size, 
                "{\"status\":\"success\",\"temp_raw\":%u,\"hum_raw\":%u}", 
                raw.temp_raw, raw.hum_raw);
        } else {
            snprintf(output, output_size, "{\"status\":\"error\",\"message\":\"SHTC3 raw read failed\"}");
        }
    } else {
        shtc3_data_t data;
        if (shtc3_read(&data) == ESP_OK) {
            snprintf(output, output_size, 
                "{\"status\":\"success\",\"temperature\":%.2f,\"humidity\":%.2f,\"unit\":\"Celsius\"}", 
                data.temperature, data.humidity);
        } else {
            snprintf(output, output_size, "{\"status\":\"error\",\"message\":\"SHTC3 sensor error\"}");
        }
    }
    
    if (root) cJSON_Delete(root);
    return ESP_OK;
}
