#include "tools/tool_bluetooth.h"
#include "hardware/bluetooth_utils.h"
#include "cJSON.h"
#include <stdio.h>
#include <string.h>
#include "esp_log.h"

esp_err_t tool_bluetooth_execute(const char *input_json, char *output, size_t output_size) {
    cJSON *root = cJSON_Parse(input_json);
    if (!root) {
        snprintf(output, output_size, "{\"status\":\"error\",\"message\":\"Invalid JSON input\"}");
        return ESP_FAIL;
    }

    cJSON *action_item = cJSON_GetObjectItem(root, "action");
    const char *action = cJSON_IsString(action_item) ? action_item->valuestring : "info";

    if (strcmp(action, "scan") == 0) {
        cJSON *dur_item = cJSON_GetObjectItem(root, "duration_ms");
        int duration = cJSON_IsNumber(dur_item) ? dur_item->valueint : 3000;
        
        /* Cap duration to prevent long blocking */
        if (duration > 10000) duration = 10000;
        if (duration < 500) duration = 500;

        return bluetooth_ble_scan(duration, output, output_size);
    } else if (strcmp(action, "toggle") == 0) {
        cJSON *state_item = cJSON_GetObjectItem(root, "state");
        const char *state = cJSON_IsString(state_item) ? state_item->valuestring : "on";
        
        esp_err_t err;
        if (strcmp(state, "on") == 0) {
            err = bluetooth_init();
        } else {
            err = bluetooth_deinit();
        }

        if (err == ESP_OK) {
            snprintf(output, output_size, "{\"status\":\"success\",\"bluetooth\":\"%s\"}", state);
        } else {
            snprintf(output, output_size, "{\"status\":\"error\",\"message\":\"Failed to toggle bluetooth\"}");
        }
        return err;
    } else if (strcmp(action, "advertise") == 0) {
        cJSON *state_item = cJSON_GetObjectItem(root, "state");
        const char *state = cJSON_IsString(state_item) ? state_item->valuestring : "on";
        
        esp_err_t err;
        if (strcmp(state, "on") == 0) {
            err = bluetooth_advertise_start();
        } else {
            err = bluetooth_advertise_stop();
        }

        if (err == ESP_OK) {
            snprintf(output, output_size, "{\"status\":\"success\",\"broadcasting\":\"%s\"}", state);
        } else {
            snprintf(output, output_size, "{\"status\":\"error\",\"message\":\"Failed to change advertising state (is bluetooth on?)\"}");
        }
        return err;
    } else if (strcmp(action, "info") == 0) {
        return bluetooth_get_info(output, output_size);
    } else {
        snprintf(output, output_size, "{\"status\":\"error\",\"message\":\"Unknown action '%s'\"}", action);
    }

    if (root) cJSON_Delete(root);
    return ESP_OK;
}
