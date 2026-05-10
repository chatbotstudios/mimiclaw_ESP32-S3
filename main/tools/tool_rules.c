#include "tools/tool_rules.h"
#include "hardware/rules_engine.h"
#include "cJSON.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "tool_rules";

esp_err_t tool_rules_execute(const char *input_json, char *output, size_t output_size) {
    cJSON *root = cJSON_Parse(input_json);
    if (!root) return ESP_ERR_INVALID_ARG;

    cJSON *action_item = cJSON_GetObjectItem(root, "action");
    if (!cJSON_IsString(action_item)) {
        cJSON_Delete(root);
        return ESP_ERR_INVALID_ARG;
    }

    const char *action = action_item->valuestring;
    esp_err_t err = ESP_OK;

    if (strcmp(action, "add") == 0) {
        const char *name = cJSON_GetObjectItem(root, "name")->valuestring;
        const char *src = cJSON_GetObjectItem(root, "src")->valuestring;
        const char *cond_str = cJSON_GetObjectItem(root, "cond")->valuestring;
        float threshold = (float)cJSON_GetObjectItem(root, "threshold")->valuedouble;
        const char *act_cmd = cJSON_GetObjectItem(root, "rule_action")->valuestring;
        
        mimi_cond_t cond = MIMI_COND_CHANGE;
        if (strcmp(cond_str, "gt") == 0) cond = MIMI_COND_GT;
        else if (strcmp(cond_str, "lt") == 0) cond = MIMI_COND_LT;
        else if (strcmp(cond_str, "eq") == 0) cond = MIMI_COND_EQ;

        err = rules_engine_add(name, src, cond, threshold, 5000, act_cmd);
        if (err == ESP_OK) {
            snprintf(output, output_size, "{\"status\":\"success\",\"message\":\"Rule '%s' added successfully.\"}", name);
        } else {
            snprintf(output, output_size, "{\"status\":\"error\",\"message\":\"Failed to add rule (limit reached?)\"}");
        }
    } else if (strcmp(action, "list") == 0) {
        char *json_rules = rules_engine_get_json();
        snprintf(output, output_size, "{\"status\":\"success\",\"rules\":%s}", json_rules);
        free(json_rules);
    } else if (strcmp(action, "remove") == 0) {
        const char *id = cJSON_GetObjectItem(root, "id")->valuestring;
        err = rules_engine_remove(id);
        if (err == ESP_OK) {
            snprintf(output, output_size, "{\"status\":\"success\",\"message\":\"Rule '%s' removed.\"}", id);
        } else {
            snprintf(output, output_size, "{\"status\":\"error\",\"message\":\"Rule ID not found.\"}");
        }
    } else {
        snprintf(output, output_size, "{\"status\":\"error\",\"message\":\"Unknown action: %s\"}", action);
        err = ESP_ERR_INVALID_ARG;
    }

    cJSON_Delete(root);
    return err;
}
