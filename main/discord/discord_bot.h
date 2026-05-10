#pragma once

#include "esp_err.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the Discord Bot configuration
 */
esp_err_t discord_bot_init(void);

/**
 * @brief Start the Discord Bot background tasks (WebSocket and REST dispatcher)
 */
esp_err_t discord_bot_start(void);

/**
 * @brief Send a message to a specific Discord channel via REST API
 * 
 * @param channel_id The Discord Channel ID (string representation of u64)
 * @param text The message content to send
 * @return esp_err_t ESP_OK on success
 */
esp_err_t discord_bot_send_message(const char *channel_id, const char *text);
esp_err_t discord_bot_send_typing(const char *channel_id);

/**
 * @brief Check if the Discord Bot is currently connected to the Gateway
 */
bool discord_bot_is_connected(void);

#ifdef __cplusplus
}
#endif
