#include "tools/tool_network.h"
#include "hardware/network_utils.h"
#include "wifi/wifi_manager.h"
#include "cJSON.h"
#include "esp_log.h"
#include <string.h>
#include <stdio.h>

// static const char *TAG = "tool_net";

esp_err_t tool_network_execute(const char *input_json, char *output, size_t output_size) {
    cJSON *root = cJSON_Parse(input_json);
    if (!root) return ESP_ERR_INVALID_ARG;

    cJSON *action_item = cJSON_GetObjectItem(root, "action");
    if (!cJSON_IsString(action_item)) {
        cJSON_Delete(root);
        return ESP_ERR_INVALID_ARG;
    }

    const char *action = action_item->valuestring;
    cJSON *resp = cJSON_CreateObject();

    if (strcmp(action, "ping") == 0) {
        cJSON *host_item = cJSON_GetObjectItem(root, "host");
        const char *host = cJSON_IsString(host_item) ? host_item->valuestring : "google.com";
        
        int ms = network_ping(host);
        if (ms >= 0) {
            cJSON_AddStringToObject(resp, "status", "success");
            cJSON_AddNumberToObject(resp, "latency_ms", ms);
            cJSON_AddStringToObject(resp, "host", host);
        } else {
            cJSON_AddStringToObject(resp, "status", "error");
            cJSON_AddStringToObject(resp, "message", "Ping failed (timeout)");
        }
    } else if (strcmp(action, "scan") == 0) {
        char scan_buf[1024];
        network_wifi_scan(scan_buf, sizeof(scan_buf));
        cJSON_AddStringToObject(resp, "status", "success");
        cJSON_AddStringToObject(resp, "scan_results", scan_buf);

    } else if (strcmp(action, "info") == 0) {
        char info_buf[256];
        if (network_get_ip_info(info_buf, sizeof(info_buf)) == ESP_OK) {
            cJSON_AddStringToObject(resp, "status", "success");
            cJSON_AddStringToObject(resp, "ip_info", info_buf);
        } else {
            cJSON_AddStringToObject(resp, "status", "error");
            cJSON_AddStringToObject(resp, "message", "Failed to fetch IP info");
        }
    } else if (strcmp(action, "rssi") == 0) {
        int rssi = wifi_manager_get_rssi();
        cJSON_AddStringToObject(resp, "status", "success");
        cJSON_AddNumberToObject(resp, "rssi_dbm", rssi);

    } else if (strcmp(action, "sync") == 0) {
        if (network_ntp_sync() == ESP_OK) {
            cJSON_AddStringToObject(resp, "status", "success");
            cJSON_AddStringToObject(resp, "message", "Clock synchronized via NTP");
        } else {
            cJSON_AddStringToObject(resp, "status", "error");
            cJSON_AddStringToObject(resp, "message", "NTP synchronization failed");
        }
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
