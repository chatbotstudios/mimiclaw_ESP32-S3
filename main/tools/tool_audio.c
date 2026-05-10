#include "tools/tool_audio.h"
#include "hardware/audio_service.h"
#include "cJSON.h"
#include <stdio.h>
#include <string.h>

esp_err_t tool_audio_play_execute(const char *input_json, char *output, size_t output_size) {
    cJSON *root = cJSON_Parse(input_json);
    if (!root) {
        snprintf(output, output_size, "Error: invalid JSON");
        return ESP_ERR_INVALID_ARG;
    }

    cJSON *file_item = cJSON_GetObjectItem(root, "file");
    if (!file_item || !cJSON_IsString(file_item)) {
        snprintf(output, output_size, "Error: missing 'file' path (e.g. /spiffs/audio/ping.raw)");
        cJSON_Delete(root);
        return ESP_ERR_INVALID_ARG;
    }

    const char *path = file_item->valuestring;
    esp_err_t err = audio_service_play_file(path);

    if (err == ESP_OK) {
        snprintf(output, output_size, "OK: played %s", path);
    } else {
        snprintf(output, output_size, "Error: could not play %s", path);
    }

    cJSON_Delete(root);
    return err;
}
