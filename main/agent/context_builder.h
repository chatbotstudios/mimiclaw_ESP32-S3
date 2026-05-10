#pragma once

#include "esp_err.h"
#include <stddef.h>

/**
 * Build the system prompt from bootstrap files (SOUL.md, USER.md)
 * and memory context (MEMORY.md + recent daily notes).
 *
 * @param buf   Output buffer (caller allocates, recommend MIMI_CONTEXT_BUF_SIZE)
 * @param size  Buffer size
 */
esp_err_t context_build_system_prompt(char *buf, size_t size);

/**
 * Build the complete messages JSON array for LLM call.
 * Combines session history + current user message.
 *
 * @param history_json    JSON array from session_get_history_json()
 * @param user_message    Current user message text
 * @param buf             Output buffer
 * @param size            Buffer size
 */
esp_err_t context_build_messages(const char *history_json, const char *user_message,
                                 char *buf, size_t size);

/**
 * Initialize the persistent Brain Cache in PSRAM.
 */
esp_err_t context_cache_init(void);

/**
 * Refresh the Brain Cache by re-reading all workspace files.
 * Call this if SOUL.md or AGENT.md are modified.
 */
esp_err_t context_cache_refresh(void);

/**
 * Copy the current cached prompt into the target buffer.
 */
esp_err_t context_get_cached_prompt(char *buf, size_t size);
