#include "tools/tool_display.h"
#include "cJSON.h"
#include "esp_log.h"
#include "hardware/epaper.h"
#include <string.h>

esp_err_t tool_display_execute(const char *input_json, char *output,
                               size_t output_size) {
  cJSON *root = cJSON_Parse(input_json);
  if (!root) {
    snprintf(output, output_size, "Error: Invalid JSON");
    return ESP_ERR_INVALID_ARG;
  }

  cJSON *text_obj = cJSON_GetObjectItem(root, "text");
  cJSON *action_obj = cJSON_GetObjectItem(root, "action");
  const char *action = action_obj ? action_obj->valuestring : "draw";

  if (strcmp(action, "clear") == 0) {
    epaper_clear();
    snprintf(output, output_size,
             "{\"status\":\"success\",\"message\":\"Display cleared\"}");
  } else if (strcmp(action, "draw") == 0) {
    const char *text = text_obj ? text_obj->valuestring : "";
    cJSON *x_obj = cJSON_GetObjectItem(root, "x");
    cJSON *y_obj = cJSON_GetObjectItem(root, "y");
    cJSON *scale_obj = cJSON_GetObjectItem(root, "scale");
    cJSON *invert_obj = cJSON_GetObjectItem(root, "invert");

    int x = cJSON_IsNumber(x_obj) ? x_obj->valueint : 10;
    int y = cJSON_IsNumber(y_obj) ? y_obj->valueint : 10;
    int scale = cJSON_IsNumber(scale_obj) ? scale_obj->valueint : 1;
    bool invert = cJSON_IsBool(invert_obj) ? cJSON_IsTrue(invert_obj) : false;

    epaper_draw_text_ext(text, x, y, scale, invert);
    snprintf(output, output_size,
             "{\"status\":\"success\",\"message\":\"Text drawn to framebuffer. "
             "Use action='push' to display it.\"}");
  } else if (strcmp(action, "push") == 0) {
    epaper_update_screen();
    snprintf(output, output_size,
             "{\"status\":\"success\",\"message\":\"Framebuffer pushed to "
             "physical screen\"}");
  } else if (strcmp(action, "refresh") == 0) {
    epaper_full_refresh();
    snprintf(output, output_size,
             "{\"status\":\"success\",\"message\":\"Full hardware refresh "
             "complete\"}");
  } else if (strcmp(action, "invert") == 0) {
    cJSON *val_obj = cJSON_GetObjectItem(root, "value");
    bool inv = cJSON_IsBool(val_obj) ? cJSON_IsTrue(val_obj) : false;
    epaper_set_invert(inv);
    snprintf(output, output_size, "{\"status\":\"success\",\"inverted\":%s}",
             inv ? "true" : "false");
  } else {
    snprintf(output, output_size,
             "{\"status\":\"error\",\"message\":\"Unknown action\"}");
  }

  cJSON_Delete(root);
  return ESP_OK;
}
