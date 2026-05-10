#ifndef AG_METRICS_H
#define AG_METRICS_H

#include "esp_err.h"
#include <stdint.h>

typedef struct {
    uint32_t tokens_in;
    uint32_t tokens_out;
    uint32_t tool_calls;
    uint32_t errors;
} agent_stats_t;

/**
 * @brief Log a tool call to the audit buffer
 */
void agent_metrics_log_tool(const char *tool, const char *action, bool success);

/**
 * @brief Record token usage
 */
void agent_metrics_add_tokens(uint32_t in, uint32_t out);

/**
 * @brief Get current session stats
 */
agent_stats_t agent_metrics_get_stats(void);

/**
 * @brief Get system uptime string
 */
void agent_metrics_get_uptime_str(char *buf, size_t size);

/**
 * @brief Get audit log as string
 */
void agent_metrics_get_audit_log(char *buf, size_t size);

#endif // AG_METRICS_H
