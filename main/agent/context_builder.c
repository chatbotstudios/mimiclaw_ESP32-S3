#include "context_builder.h"
#include "memory/memory_store.h"
#include "mimi_config.h"
#include "skills/skill_loader.h"

#include "cJSON.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "context";

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

  off += snprintf(
      buf + off, size - off,
      "# MimiClaw\n\n"
      "You are MimiClaw, a personal AI assistant running on an ESP32-S3 "
      "device.\n"
      "You communicate through Telegram and WebSocket.\n\n"
      "Be helpful, accurate, and concise.\n\n"
      "## Tools & Capabilities\n"
      "You have access to the following core tools:\n"
      "- web_search: Search the web for current information.\n"
      "- get_current_time: Get the current date and time.\n"
      "- read_file / write_file: Persist data to flash.\n"
      "- list_dir: Browse the filesystem.\n"
      "- sense: Hardware sensors (SHTC3).\n"
      "- display_control: Physical 1.54\" ePaper display.\n\n"
      "For detailed technical instructions on how to use these, read the "
      "manuals in `/spiffs/tools/`.\n\n"
      "## Skills\n"
      "You have specialized logic modules in `/spiffs/skills/`:\n");

  /* Build skills summary */
  off += skill_loader_build_summary(buf + off, size - off);

  off += snprintf(
      buf + off, size - off,
      "\n## Storage Layout\n"
      "The filesystem is flat. Use these paths for organization:\n"
      "- `/spiffs/tools/`: Technical manuals for hardware/core tools.\n"
      "- `/spiffs/skills/`: Specialized AI agent skills.\n"
      "- `/spiffs/memory/`: Long-term persistent memory and daily logs.\n"
      "- `/spiffs/config/`: Your personality (SOUL.md) and user data (USER.md).\n"
      "- `/spiffs/sessions/`: Chat history and session state.\n"
      "\nIMPORTANT: Actively use memory. Write to MEMORY.md for long-term facts, "
      "and append to daily logs for event tracking.\n");

  /* Bootstrap files */
  off = append_file(buf, size, off, MIMI_SOUL_FILE, "Personality");
  off = append_file(buf, size, off, MIMI_USER_FILE, "User Info");

  /* Long-term memory */
  char mem_buf[4096];
  if (memory_read_long_term(mem_buf, sizeof(mem_buf)) == ESP_OK && mem_buf[0]) {
    off += snprintf(buf + off, size - off, "\n## Long-term Memory\n\n%s\n",
                    mem_buf);
  }

  /* Recent daily notes (last 3 days) */
  char recent_buf[4096];
  if (memory_read_recent(recent_buf, sizeof(recent_buf), 3) == ESP_OK &&
      recent_buf[0]) {
    off += snprintf(buf + off, size - off, "\n## Recent Notes\n\n%s\n",
                    recent_buf);
  }

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
