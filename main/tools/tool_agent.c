#include "tools/tool_agent.h"
#include "agent/agent_metrics.h"
#include "agent/agent_loop.h"
#include "cJSON.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <stdio.h>

// static const char *TAG = "tool_ag";

esp_err_t tool_agent_execute(const char *input_json, char *output, size_t output_size) {
    cJSON *root = cJSON_Parse(input_json);
    if (!root) return ESP_ERR_INVALID_ARG;

    cJSON *action_item = cJSON_GetObjectItem(root, "action");
    if (!cJSON_IsString(action_item)) {
        cJSON_Delete(root);
        return ESP_ERR_INVALID_ARG;
    }

    const char *action = action_item->valuestring;
    cJSON *resp = cJSON_CreateObject();

    if (strcmp(action, "metrics") == 0) {
        agent_stats_t stats = agent_metrics_get_stats();
        cJSON_AddStringToObject(resp, "status", "success");
        cJSON_AddNumberToObject(resp, "tokens_in", stats.tokens_in);
        cJSON_AddNumberToObject(resp, "tokens_out", stats.tokens_out);
        cJSON_AddNumberToObject(resp, "tool_calls", stats.tool_calls);
        cJSON_AddNumberToObject(resp, "errors", stats.errors);

    } else if (strcmp(action, "audit") == 0) {
        char audit_buf[1024];
        agent_metrics_get_audit_log(audit_buf, sizeof(audit_buf));
        cJSON_AddStringToObject(resp, "status", "success");
        cJSON_AddStringToObject(resp, "audit_log", audit_buf);

    } else if (strcmp(action, "stack") == 0) {
        TaskHandle_t h = agent_loop_get_task_handle();
        if (h) {
            uint32_t watermark = (uint32_t)uxTaskGetStackHighWaterMark(h);
            cJSON_AddStringToObject(resp, "status", "success");
            cJSON_AddNumberToObject(resp, "stack_free_bytes", watermark);
        } else {
            cJSON_AddStringToObject(resp, "status", "error");
            cJSON_AddStringToObject(resp, "message", "Agent task not found");
        }
    } else if (strcmp(action, "uptime") == 0) {
        char uptime_buf[64];
        agent_metrics_get_uptime_str(uptime_buf, sizeof(uptime_buf));
        cJSON_AddStringToObject(resp, "status", "success");
        cJSON_AddStringToObject(resp, "uptime", uptime_buf);

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
