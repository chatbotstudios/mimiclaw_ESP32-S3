#include "tools/tool_led.h"
#include "hardware/led.h"
#include "cJSON.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

esp_err_t tool_led_control_execute(const char *input_json, char *output, size_t output_size)
{
    cJSON *root = cJSON_Parse(input_json);
    if (!root) {
        snprintf(output, output_size, "Error: invalid JSON input");
        return ESP_ERR_INVALID_ARG;
    }

    cJSON *action_item = cJSON_GetObjectItem(root, "action");
    const char *action = action_item ? action_item->valuestring : "on";

    /* Determine Color */
    uint32_t color_hex = 0x00FF00; // Default Green

    cJSON *hex_item = cJSON_GetObjectItem(root, "hex");
    cJSON *r_item = cJSON_GetObjectItem(root, "r");
    cJSON *g_item = cJSON_GetObjectItem(root, "g");
    cJSON *b_item = cJSON_GetObjectItem(root, "b");
    cJSON *color_name = cJSON_GetObjectItem(root, "color");

    if (hex_item && cJSON_IsString(hex_item)) {
        color_hex = (uint32_t)strtol(hex_item->valuestring + (hex_item->valuestring[0] == '#' ? 1 : 0), NULL, 16);
    } else if (r_item && g_item && b_item) {
        color_hex = ((r_item->valueint & 0xFF) << 16) | ((g_item->valueint & 0xFF) << 8) | (b_item->valueint & 0xFF);
    } else if (color_name && cJSON_IsString(color_name)) {
        const char *name = color_name->valuestring;
        if (strcmp(name, "red") == 0) color_hex = 0xFF0000;
        else if (strcmp(name, "green") == 0) color_hex = 0x00FF00;
        else if (strcmp(name, "blue") == 0) color_hex = 0x0000FF;
        else if (strcmp(name, "purple") == 0) color_hex = 0x800080;
        else if (strcmp(name, "yellow") == 0) color_hex = 0xFFFF00;
        else if (strcmp(name, "orange") == 0) color_hex = 0xFFA500;
        else if (strcmp(name, "cyan") == 0) color_hex = 0x00FFFF;
        else if (strcmp(name, "white") == 0) color_hex = 0xFFFFFF;
    }

    if (strcmp(action, "off") == 0) {
        led_set_rgb(0, 0, 0);
        snprintf(output, output_size, "OK: RGB LED turned off");
    } else {
        led_set_color(color_hex);
        snprintf(output, output_size, "OK: RGB LED set to 0x%06X", (unsigned int)color_hex);
    }

    cJSON_Delete(root);
    return ESP_OK;
}
