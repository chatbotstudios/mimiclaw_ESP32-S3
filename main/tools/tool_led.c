#include "tools/tool_led.h"
#include "hardware/led.h"
#include "cJSON.h"
#include <stdio.h>
#include <string.h>

esp_err_t tool_led_control_execute(const char *input_json, char *output, size_t output_size)
{
    cJSON *root = cJSON_Parse(input_json);
    if (!root) {
        snprintf(output, output_size, "Error: invalid JSON input");
        return ESP_ERR_INVALID_ARG;
    }

    cJSON *action_item = cJSON_GetObjectItem(root, "action");
    if (!action_item || !cJSON_IsString(action_item)) {
        snprintf(output, output_size, "Error: missing 'action' string (on, off, blink)");
        cJSON_Delete(root);
        return ESP_ERR_INVALID_ARG;
    }

    const char *action = action_item->valuestring;
    mimi_led_color_t color = MIMI_LED_RED;
    cJSON *color_item = cJSON_GetObjectItem(root, "color");
    if (color_item && cJSON_IsString(color_item)) {
        if (strcmp(color_item->valuestring, "green") == 0) {
            color = MIMI_LED_GREEN;
        }
    }

    if (strcmp(action, "on") == 0) {
        led_set_level(color, 1);
        snprintf(output, output_size, "OK: %s LED turned on", color == MIMI_LED_RED ? "Red" : "Green");
    } else if (strcmp(action, "off") == 0) {
        led_set_level(color, 0);
        snprintf(output, output_size, "OK: %s LED turned off", color == MIMI_LED_RED ? "Red" : "Green");
    } else if (strcmp(action, "blink") == 0) {
        int duration = 500;
        cJSON *ms_item = cJSON_GetObjectItem(root, "ms");
        if (ms_item && cJSON_IsNumber(ms_item)) {
            duration = ms_item->valueint;
        }
        led_blink(color, duration);
        snprintf(output, output_size, "OK: %s LED blinked for %d ms", color == MIMI_LED_RED ? "Red" : "Green", duration);
    } else {
        snprintf(output, output_size, "Error: unknown action '%s'", action);
        cJSON_Delete(root);
        return ESP_ERR_INVALID_ARG;
    }

    cJSON_Delete(root);
    return ESP_OK;
}
