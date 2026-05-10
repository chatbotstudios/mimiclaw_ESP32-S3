#include "tools/tool_cli.h"
#include "cJSON.h"
#include "esp_console.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static const char *TAG = "tool_cli";
static SemaphoreHandle_t s_cli_mutex = NULL;

static const char *s_blocklist[] = {
    "config_reset",
    "wifi_set",
    "tg_token",
    "api_key",
    "set_api_key",
    "set_proxy",
    "clear_proxy",
    "set_search_key",
    "file_rm",
    "mkdir",
    "mv",
    "cp",
    "touch",
    "file_put",
    "file_b64",
    "file_b64_append",
    "restart",
    "deep_sleep",
    NULL
};

static bool is_command_blocked(const char *cmd) {
  for (int i = 0; s_blocklist[i] != NULL; i++) {
    size_t len = strlen(s_blocklist[i]);
    if (strncmp(cmd, s_blocklist[i], len) == 0 && (cmd[len] == ' ' || cmd[len] == '\0')) {
      return true;
    }
  }
  return false;
}

esp_err_t tool_cli_execute(const char *input_json, char *output,
                           size_t output_size) {
  cJSON *root = cJSON_Parse(input_json);
  if (!root)
    return ESP_ERR_INVALID_ARG;

  cJSON *cmd_item = cJSON_GetObjectItem(root, "command");
  if (!cJSON_IsString(cmd_item)) {
    cJSON_Delete(root);
    return ESP_ERR_INVALID_ARG;
  }

  const char *cmd_str = cmd_item->valuestring;
  
  /* Skip leading whitespace */
  while (*cmd_str == ' ') cmd_str++;

  if (is_command_blocked(cmd_str)) {
    ESP_LOGW(TAG, "Blocked sensitive CLI command: %s", cmd_str);
    snprintf(output, output_size, "Error: command '%s' is blocked for security reasons. Use dedicated tools for these actions.", cmd_str);
    cJSON_Delete(root);
    return ESP_ERR_INVALID_ARG;
  }

  ESP_LOGI(TAG, "Mimi executing CLI command: %s", cmd_str);

  /* 1. Open a memory stream for capturing output */
  char *capture_buf = NULL;
  size_t capture_size = 0;
  FILE *mem_file = open_memstream(&capture_buf, &capture_size);
  if (!mem_file) {
    cJSON_Delete(root);
    return ESP_ERR_NO_MEM;
  }

  /* 2. Save original stdout and swap to mem_file (Thread Safe) */
  if (!s_cli_mutex)
    s_cli_mutex = xSemaphoreCreateMutex();
  xSemaphoreTake(s_cli_mutex, portMAX_DELAY);

  FILE *orig_stdout = stdout;
  stdout = mem_file;

  /* 3. Run the console command */
  int ret_code = 0;
  esp_err_t err = esp_console_run(cmd_str, &ret_code);

  /* 4. Flush and restore stdout */
  fflush(stdout);
  stdout = orig_stdout;
  xSemaphoreGive(s_cli_mutex);
  fclose(mem_file);

  /* 5. Prepare tool output */
  if (err != ESP_OK) {
    snprintf(output, output_size,
             "{\"status\":\"error\",\"code\":%d,\"message\":\"%s\"}", (int)err,
             esp_err_to_name(err));
  } else {
    /* Cap the output to fit in the response buffer */
    if (capture_buf && capture_size > 0) {
      /* Create a valid JSON response */
      cJSON *resp = cJSON_CreateObject();
      cJSON_AddStringToObject(resp, "status", "success");
      cJSON_AddNumberToObject(resp, "return_code", ret_code);

      /* Truncate if too large for output_size */
      if (capture_size > output_size - 128) {
        capture_buf[output_size - 128] = '\0';
        strcat(capture_buf, "... [truncated]");
      }
      cJSON_AddStringToObject(resp, "output", capture_buf);

      char *json_out = cJSON_PrintUnformatted(resp);
      strncpy(output, json_out, output_size - 1);
      output[output_size - 1] = '\0';

      free(json_out);
      cJSON_Delete(resp);
    } else {
      snprintf(output, output_size,
               "{\"status\":\"success\",\"return_code\":%d,\"output\":\"(no "
               "output)\"}",
               ret_code);
    }
  }

  free(capture_buf);
  cJSON_Delete(root);
  return ESP_OK;
}
