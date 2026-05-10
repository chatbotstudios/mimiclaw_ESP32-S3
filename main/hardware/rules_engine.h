#ifndef MIMI_RULES_ENGINE_H
#define MIMI_RULES_ENGINE_H

#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>

#define MAX_MIMI_RULES 16
#define RULE_ID_LEN 16
#define RULE_NAME_LEN 32
#define RULE_SRC_LEN 16
#define RULE_ACTION_LEN 64

typedef enum {
    MIMI_COND_GT,
    MIMI_COND_LT,
    MIMI_COND_EQ,
    MIMI_COND_CHANGE
} mimi_cond_t;

typedef struct {
    char id[RULE_ID_LEN];
    char name[RULE_NAME_LEN];
    char trigger_src[RULE_SRC_LEN]; // "temp", "hum", "batt", "uptime"
    mimi_cond_t cond;
    float threshold;
    uint32_t interval_ms;
    char action_cmd[RULE_ACTION_LEN]; // e.g., "color red", "cli restart"
    bool enabled;
    bool fired;       // Edge-triggering state
    uint32_t last_eval;
    float last_val;
} mimi_rule_t;

/**
 * @brief Initialize the rule engine and load rules from SPIFFS.
 */
esp_err_t rules_engine_init(void);

/**
 * @brief Evaluate all active rules against current sensor data.
 * Called periodically from the agent loop.
 */
void rules_engine_evaluate(void);

/**
 * @brief Add a new rule to the engine and save to SPIFFS.
 */
esp_err_t rules_engine_add(const char *name, const char *src, mimi_cond_t cond, 
                           float threshold, uint32_t interval, const char *action);

/**
 * @brief Remove a rule by ID.
 */
esp_err_t rules_engine_remove(const char *id);

/**
 * @brief Get all rules as a JSON string (for the tool response).
 */
char* rules_engine_get_json(void);

#endif // MIMI_RULES_ENGINE_H
