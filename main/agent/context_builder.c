#include "context_builder.h"
#include "memory/memory_store.h"
#include "mimi_config.h"
#include "skills/skill_loader.h"

#include "cJSON.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static const char *TAG = "context";
static char *s_brain_cache = NULL;
static size_t s_brain_cache_len = 0;
static SemaphoreHandle_t s_cache_mux = NULL;

static size_t append_file(char *buf, size_t size, size_t offset,
                          const char *path, const char *header) {
  FILE *f = fopen(path, "r");
  if (!f)
    return offset;

  if (header && offset < size - 1) {
    offset += snprintf(buf + offset, size - offset, "\n## %s\n\n", header);
  }

  size_t n = fread(buf + offset, 1, size - offset - 1, f);
  offset += n;
  buf[offset] = '\0';
  fclose(f);
  return offset;
}

esp_err_t context_build_system_prompt(char *buf, size_t size) {
  size_t off = 0;

  /* 1. Identity & Persona */
  off += snprintf(buf + off, size - off, "# MimiClaw Workspace\n\n");
  off = append_file(buf, size, off, MIMI_IDENTITY_FILE, "Identity");
  off = append_file(buf, size, off, MIMI_SOUL_FILE, "Soul");

  /* 2. Operating Rules */
  off = append_file(buf, size, off, MIMI_AGENT_FILE, "Agent Rules");

  /* 3. User Context */
  off = append_file(buf, size, off, MIMI_USER_FILE, "User Profile");

  /* 4. Capabilities & Environment */
  off = append_file(buf, size, off, MIMI_TOOLS_FILE, "Tool Conventions");
  
  off += snprintf(
      buf + off, size - off,
      "\n## Core Skills\n"
      "You have specialized logic modules in `%s`:\n", MIMI_SKILLS_PREFIX);

  /* Build skills summary */
  off += skill_loader_build_summary(buf + off, size - off);

  off += snprintf(
      buf + off, size - off,
      "\n## Storage Layout\n"
      "The filesystem follows the OpenClaw Workspace standard:\n"
      "- `%s`: Identity, Persona, Rules, User Context, and Tools.\n"
      "- `%s`: Modular AI agent skills.\n"
      "- `%s`: Long-term memory (MEMORY.md) and daily history logs.\n"
      "- `%s`: Active chat history and session state.\n",
      MIMI_WORKSPACE_DIR, MIMI_SKILLS_PREFIX, MIMI_HISTORY_PREFIX, MIMI_SESSION_DIR);

  /* 5. Memory & History */
  off = append_file(buf, size, off, MIMI_MEMORY_FILE, "Long-term Memory");

  /* Recent history logs (last 3 days) */
  // Note: We would ideally use memory_read_recent here, but updated to history path
  // For now, keep the header and a note
  off += snprintf(buf + off, size - off, "\n## Recent History\n(Recent logs from %s)\n", MIMI_HISTORY_PREFIX);

  ESP_LOGI(TAG, "System prompt built: %d bytes", (int)off);
  return ESP_OK;
}

esp_err_t context_build_messages(const char *history_json,
                                 const char *user_message, char *buf,
                                 size_t size) {
  /* Parse existing history */
  cJSON *history = cJSON_Parse(history_json);
  if (!history) {
    history = cJSON_CreateArray();
  }

  /* Append current user message */
  cJSON *user_msg = cJSON_CreateObject();
  cJSON_AddStringToObject(user_msg, "role", "user");
  cJSON_AddStringToObject(user_msg, "content", user_message);
  cJSON_AddItemToArray(history, user_msg);

  /* Serialize */
  char *json_str = cJSON_PrintUnformatted(history);
  cJSON_Delete(history);

  if (json_str) {
    strncpy(buf, json_str, size - 1);
    buf[size - 1] = '\0';
    free(json_str);
  } else {
    snprintf(buf, size, "[{\"role\":\"user\",\"content\":\"%s\"}]",
             user_message);
  }

  return ESP_OK;
}
esp_err_t context_cache_init(void) {
  if (s_cache_mux == NULL) {
    s_cache_mux = xSemaphoreCreateMutex();
  }

  if (s_brain_cache == NULL) {
    s_brain_cache = heap_caps_malloc(MIMI_CONTEXT_BUF_SIZE, MALLOC_CAP_SPIRAM);
    if (!s_brain_cache) {
      ESP_LOGE(TAG, "Failed to allocate Brain Cache in PSRAM");
      return ESP_ERR_NO_MEM;
    }
  }

  return context_cache_refresh();
}

esp_err_t context_cache_refresh(void) {
  if (!s_brain_cache || !s_cache_mux)
    return ESP_ERR_INVALID_STATE;

  xSemaphoreTake(s_cache_mux, portMAX_DELAY);
  memset(s_brain_cache, 0, MIMI_CONTEXT_BUF_SIZE);
  esp_err_t err = context_build_system_prompt(s_brain_cache, MIMI_CONTEXT_BUF_SIZE);
  if (err == ESP_OK) {
    s_brain_cache_len = strlen(s_brain_cache);
    ESP_LOGI(TAG, "Brain Cache refreshed (%d bytes)", (int)s_brain_cache_len);
  }
  xSemaphoreGive(s_cache_mux);

  return err;
}

esp_err_t context_get_cached_prompt(char *buf, size_t size) {
  if (!s_brain_cache || !s_cache_mux)
    return ESP_ERR_INVALID_STATE;

  xSemaphoreTake(s_cache_mux, portMAX_DELAY);
  strncpy(buf, s_brain_cache, size - 1);
  buf[size - 1] = '\0';
  xSemaphoreGive(s_cache_mux);

  return ESP_OK;
}
