#include "agent/agent_metrics.h"
#include "esp_timer.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

// static const char *TAG = "ag_metrics";

#define MAX_AUDIT_LOGS 10
#define LOG_ENTRY_SIZE 128

static agent_stats_t s_stats = {0};
static char s_audit_buffer[MAX_AUDIT_LOGS][LOG_ENTRY_SIZE];
static int s_audit_head = 0;
static int s_audit_count = 0;

void agent_metrics_log_tool(const char *tool, const char *action, bool success) {
    s_stats.tool_calls++;
    if (!success) s_stats.errors++;

    time_t now;
    time(&now);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    snprintf(s_audit_buffer[s_audit_head], LOG_ENTRY_SIZE, 
             "[%02d:%02d:%02d] %s(%s) -> %s", 
             timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec,
             tool, action ? action : "", success ? "OK" : "FAIL");

    s_audit_head = (s_audit_head + 1) % MAX_AUDIT_LOGS;
    if (s_audit_count < MAX_AUDIT_LOGS) s_audit_count++;
}

void agent_metrics_add_tokens(uint32_t in, uint32_t out) {
    s_stats.tokens_in += in;
    s_stats.tokens_out += out;
}

agent_stats_t agent_metrics_get_stats(void) {
    return s_stats;
}

void agent_metrics_get_uptime_str(char *buf, size_t size) {
    int64_t uptime_us = esp_timer_get_time();
    int64_t uptime_s = uptime_us / 1000000;
    
    int days = uptime_s / (24 * 3600);
    int hours = (uptime_s % (24 * 3600)) / 3600;
    int mins = (uptime_s % 3600) / 60;


    snprintf(buf, size, "%02d:%02d:%02d", days, hours, mins);
}

void agent_metrics_get_audit_log(char *buf, size_t size) {
    size_t off = 0;
    off += snprintf(buf + off, size - off, "Recent Tool Calls:\n");
    
    for (int i = 0; i < s_audit_count; i++) {
        int idx = (s_audit_head - 1 - i + MAX_AUDIT_LOGS) % MAX_AUDIT_LOGS;
        off += snprintf(buf + off, size - off, "- %s\n", s_audit_buffer[idx]);
        if (off >= size - 1) break;
    }
}
