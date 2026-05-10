#include "tools/tool_power.h"
#include "cJSON.h"
#include "esp_log.h"
#include "hardware/battery.h"
#include "hardware/pm_system.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "tool_pwr";

esp_err_t tool_power_execute(const char *input_json, char *output,
                             size_t output_size) {
  cJSON *root = cJSON_Parse(input_json);
  if (!root)
    return ESP_ERR_INVALID_ARG;

  cJSON *action_item = cJSON_GetObjectItem(root, "action");
  if (!cJSON_IsString(action_item)) {
    cJSON_Delete(root);
    return ESP_ERR_INVALID_ARG;
  }

  const char *action = action_item->valuestring;
  cJSON *resp = cJSON_CreateObject();

  if (strcmp(action, "status") == 0) {
    float v = battery_get_voltage();
    int p = battery_get_percentage();
    mimi_pwr_mode_t m = pm_system_get_mode();
    const char *mode_names[] = {"balanced", "performance"};

    cJSON_AddStringToObject(resp, "status", "success");
    cJSON_AddNumberToObject(resp, "battery_pct", p);
    cJSON_AddNumberToObject(resp, "voltage", v);
    cJSON_AddStringToObject(resp, "current_mode", mode_names[m]);

  } else if (strcmp(action, "set_mode") == 0) {
    cJSON *mode_item = cJSON_GetObjectItem(root, "mode");
    if (cJSON_IsString(mode_item)) {
      const char *mode_str = mode_item->valuestring;
      esp_err_t err = ESP_OK;

      if (strcmp(mode_str, "balanced") == 0)
        err = pm_system_set_mode(MIMI_PWR_BALANCED);
      else if (strcmp(mode_str, "performance") == 0)
        err = pm_system_set_mode(MIMI_PWR_PERFORMANCE);
      else
        err = ESP_ERR_INVALID_ARG;

      if (err == ESP_OK) {
        cJSON_AddStringToObject(resp, "status", "success");
        cJSON_AddStringToObject(resp, "message", "Power mode updated");
      } else {
        cJSON_AddStringToObject(resp, "status", "error");
        cJSON_AddStringToObject(resp, "message",
                                "Invalid mode or hardware error");
      }
    }
  } else if (strcmp(action, "hibernate") == 0) {
    cJSON *duration_item = cJSON_GetObjectItem(root, "duration_ms");
    uint64_t ms = cJSON_IsNumber(duration_item)
                      ? (uint64_t)duration_item->valuedouble
                      : 3600000; // Default 1hr

    cJSON_AddStringToObject(resp, "status", "success");
    cJSON_AddStringToObject(resp, "message", "Entering deep sleep...");

    char *json_out = cJSON_PrintUnformatted(resp);
    strncpy(output, json_out, output_size - 1);
    free(json_out);
    cJSON_Delete(resp);
    cJSON_Delete(root);

    /* Trigger sleep after a short delay to allow response to be sent */
    ESP_LOGI(TAG, "AI triggered hibernation for %llu ms", ms);
    pm_system_deep_sleep(ms);
    return ESP_OK;

  } else {
    cJSON_AddStringToObject(resp, "status", "error");
    cJSON_AddStringToObject(resp, "message", "Unknown action");
  }

  char *json_out = cJSON_PrintUnformatted(resp);
  strncpy(output, json_out, output_size - 1);
  output[output_size - 1] = '\0';

  free(json_out);
  cJSON_Delete(resp);
  cJSON_Delete(root);
  return ESP_OK;
}
