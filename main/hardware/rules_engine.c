#include "hardware/rules_engine.h"
#include "hardware/shtc3.h"
#include "hardware/battery.h"
#include "hardware/led.h"
#include "esp_log.h"
#include "esp_console.h"
#include "cJSON.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "rules_engine";
static mimi_rule_t s_rules[MAX_MIMI_RULES];
static int s_rule_count = 0;
static uint32_t s_rule_id_counter = 0;

/* Helper to convert condition string to enum */
static mimi_cond_t cond_from_str(const char *str) {
    if (strcmp(str, "gt") == 0) return MIMI_COND_GT;
    if (strcmp(str, "lt") == 0) return MIMI_COND_LT;
    if (strcmp(str, "eq") == 0) return MIMI_COND_EQ;
    return MIMI_COND_CHANGE;
}

static const char* cond_to_str(mimi_cond_t cond) {
    switch (cond) {
        case MIMI_COND_GT: return "gt";
        case MIMI_COND_LT: return "lt";
        case MIMI_COND_EQ: return "eq";
        default: return "change";
    }
}

/* Save rules to SPIFFS */
static void rules_save_to_spiffs(void) {
    cJSON *root = cJSON_CreateArray();
    for (int i = 0; i < MAX_MIMI_RULES; i++) {
        if (!s_rules[i].enabled && s_rules[i].id[0] == '\0') continue;
        
        cJSON *item = cJSON_CreateObject();
        cJSON_AddStringToObject(item, "id", s_rules[i].id);
        cJSON_AddStringToObject(item, "name", s_rules[i].name);
        cJSON_AddStringToObject(item, "src", s_rules[i].trigger_src);
        cJSON_AddStringToObject(item, "cond", cond_to_str(s_rules[i].cond));
        cJSON_AddNumberToObject(item, "th", s_rules[i].threshold);
        cJSON_AddNumberToObject(item, "iv", s_rules[i].interval_ms);
        cJSON_AddStringToObject(item, "act", s_rules[i].action_cmd);
        cJSON_AddItemToArray(root, item);
    }

    char *json_str = cJSON_PrintUnformatted(root);
    FILE *f = fopen("/spiffs/rules.json", "w");
    if (f) {
        fputs(json_str, f);
        fclose(f);
        ESP_LOGI(TAG, "Rules saved to SPIFFS (%d active)", s_rule_count);
    }
    free(json_str);
    cJSON_Delete(root);
}

esp_err_t rules_engine_init(void) {
    memset(s_rules, 0, sizeof(s_rules));
    
    FILE *f = fopen("/spiffs/rules.json", "r");
    if (!f) {
        ESP_LOGI(TAG, "No existing rules.json found.");
        return ESP_OK;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc(size + 1);
    fread(buf, 1, size, f);
    buf[size] = '\0';
    fclose(f);

    cJSON *root = cJSON_Parse(buf);
    if (root && cJSON_IsArray(root)) {
        int count = cJSON_GetArraySize(root);
        for (int i = 0; i < count && i < MAX_MIMI_RULES; i++) {
            cJSON *item = cJSON_GetArrayItem(root, i);
            mimi_rule_t *r = &s_rules[i];
            
            strncpy(r->id, cJSON_GetObjectItem(item, "id")->valuestring, RULE_ID_LEN-1);
            strncpy(r->name, cJSON_GetObjectItem(item, "name")->valuestring, RULE_NAME_LEN-1);
            strncpy(r->trigger_src, cJSON_GetObjectItem(item, "src")->valuestring, RULE_SRC_LEN-1);
            r->cond = cond_from_str(cJSON_GetObjectItem(item, "cond")->valuestring);
            r->threshold = (float)cJSON_GetObjectItem(item, "th")->valuedouble;
            r->interval_ms = (uint32_t)cJSON_GetObjectItem(item, "iv")->valueint;
            strncpy(r->action_cmd, cJSON_GetObjectItem(item, "act")->valuestring, RULE_ACTION_LEN-1);
            r->enabled = true;
            
            /* Update ID counter */
            int id_num = atoi(r->id + 2); // Skip "R_"
            if (id_num > s_rule_id_counter) s_rule_id_counter = id_num;
            s_rule_count++;
        }
    }
    
    if (root) cJSON_Delete(root);
    free(buf);
    ESP_LOGI(TAG, "Loaded %d rules from SPIFFS", s_rule_count);
    return ESP_OK;
}

static void execute_rule_action(mimi_rule_t *r) {
    ESP_LOGI(TAG, "Triggering rule '%s': %s", r->name, r->action_cmd);
    
    /* 1. Check for 'color' actions directly for speed */
    if (strncmp(r->action_cmd, "color ", 6) == 0) {
        /* This is a mood override */
        int ret_code;
        esp_console_run(r->action_cmd, &ret_code);
    } else {
        /* Fallback: run via console */
        int ret_code;
        esp_console_run(r->action_cmd, &ret_code);
    }
}

void rules_engine_evaluate(void) {
    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
    shtc3_data_t sd = {0};
    shtc3_read(&sd);
    float batt = battery_get_voltage();

    for (int i = 0; i < MAX_MIMI_RULES; i++) {
        mimi_rule_t *r = &s_rules[i];
        if (!r->enabled || r->id[0] == '\0') continue;

        if (now - r->last_eval < r->interval_ms) continue;
        r->last_eval = now;

        float val = 0;
        if (strcmp(r->trigger_src, "temp") == 0) val = sd.temperature;
        else if (strcmp(r->trigger_src, "hum") == 0) val = sd.humidity;
        else if (strcmp(r->trigger_src, "batt") == 0) val = batt;
        else if (strcmp(r->trigger_src, "uptime") == 0) val = (float)(now / 1000);
        else continue;

        bool met = false;
        switch (r->cond) {
            case MIMI_COND_GT: met = (val > r->threshold); break;
            case MIMI_COND_LT: met = (val < r->threshold); break;
            case MIMI_COND_EQ: met = (val == r->threshold); break;
            case MIMI_COND_CHANGE: met = (val != r->last_val); break;
        }

        if (met && !r->fired) {
            r->fired = true;
            execute_rule_action(r);
        } else if (!met && r->fired) {
            r->fired = false; // Reset for next trigger
        }
        
        r->last_val = val;
    }
}

esp_err_t rules_engine_add(const char *name, const char *src, mimi_cond_t cond, 
                           float threshold, uint32_t interval, const char *action) {
    int slot = -1;
    for (int i = 0; i < MAX_MIMI_RULES; i++) {
        if (s_rules[i].id[0] == '\0') {
            slot = i;
            break;
        }
    }

    if (slot == -1) return ESP_ERR_NO_MEM;

    mimi_rule_t *r = &s_rules[slot];
    s_rule_id_counter++;
    snprintf(r->id, RULE_ID_LEN, "R_%lu", s_rule_id_counter);
    strncpy(r->name, name, RULE_NAME_LEN-1);
    strncpy(r->trigger_src, src, RULE_SRC_LEN-1);
    r->cond = cond;
    r->threshold = threshold;
    r->interval_ms = interval;
    strncpy(r->action_cmd, action, RULE_ACTION_LEN-1);
    r->enabled = true;
    r->fired = false;
    r->last_eval = 0;
    
    s_rule_count++;
    rules_save_to_spiffs();
    return ESP_OK;
}

esp_err_t rules_engine_remove(const char *id) {
    for (int i = 0; i < MAX_MIMI_RULES; i++) {
        if (strcmp(s_rules[i].id, id) == 0) {
            memset(&s_rules[i], 0, sizeof(mimi_rule_t));
            s_rule_count--;
            rules_save_to_spiffs();
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}

char* rules_engine_get_json(void) {
    cJSON *root = cJSON_CreateArray();
    for (int i = 0; i < MAX_MIMI_RULES; i++) {
        if (s_rules[i].id[0] == '\0') continue;
        cJSON *item = cJSON_CreateObject();
        cJSON_AddStringToObject(item, "id", s_rules[i].id);
        cJSON_AddStringToObject(item, "name", s_rules[i].name);
        cJSON_AddStringToObject(item, "src", s_rules[i].trigger_src);
        cJSON_AddStringToObject(item, "cond", cond_to_str(s_rules[i].cond));
        cJSON_AddNumberToObject(item, "val", s_rules[i].threshold);
        cJSON_AddStringToObject(item, "action", s_rules[i].action_cmd);
        cJSON_AddItemToArray(root, item);
    }
    char *out = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return out;
}
