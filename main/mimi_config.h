#pragma once

/* MimiClaw Global Configuration */

/* Build-time secrets (highest priority, override NVS) */
#if __has_include("mimi_secrets.h")
#include "mimi_secrets.h"
#endif

#ifndef MIMI_SECRET_WIFI_SSID
#define MIMI_SECRET_WIFI_SSID ""
#endif
#ifndef MIMI_SECRET_WIFI_PASS
#define MIMI_SECRET_WIFI_PASS ""
#endif
#ifndef MIMI_SECRET_TG_TOKEN
#define MIMI_SECRET_TG_TOKEN ""
#endif
#ifndef MIMI_SECRET_DISCORD_TOKEN
#define MIMI_SECRET_DISCORD_TOKEN ""
#endif
#ifndef MIMI_SECRET_DISCORD_USER_ID
#define MIMI_SECRET_DISCORD_USER_ID ""
#endif
#ifndef MIMI_SECRET_API_KEY
#define MIMI_SECRET_API_KEY ""
#endif
#ifndef MIMI_SECRET_MODEL
#define MIMI_SECRET_MODEL ""
#endif
#ifndef MIMI_SECRET_MODEL_PROVIDER
#define MIMI_SECRET_MODEL_PROVIDER "anthropic"
#endif
#ifndef MIMI_SECRET_PROXY_HOST
#define MIMI_SECRET_PROXY_HOST ""
#endif
#ifndef MIMI_SECRET_PROXY_PORT
#define MIMI_SECRET_PROXY_PORT ""
#endif
#ifndef MIMI_SECRET_SEARCH_KEY
#define MIMI_SECRET_SEARCH_KEY ""
#endif
#ifndef MIMI_SECRET_API_KEY_ANT
#define MIMI_SECRET_API_KEY_ANT ""
#endif
#ifndef MIMI_SECRET_API_KEY_GEM
#define MIMI_SECRET_API_KEY_GEM ""
#endif
#ifndef MIMI_SECRET_API_KEY_OAI
#define MIMI_SECRET_API_KEY_OAI ""
#endif
#ifndef MIMI_SECRET_MODEL_ANT
#define MIMI_SECRET_MODEL_ANT "claude-3-5-sonnet-20241022"
#endif
#ifndef MIMI_SECRET_MODEL_GEM
#define MIMI_SECRET_MODEL_GEM "gemini-flash-latest"
#endif
#ifndef MIMI_SECRET_MODEL_OAI
#define MIMI_SECRET_MODEL_OAI "gpt-4o"
#endif

/* WiFi */
#define MIMI_WIFI_MAX_RETRY 10
#define MIMI_WIFI_RETRY_BASE_MS 1000
#define MIMI_WIFI_RETRY_MAX_MS 30000

/* Telegram Bot */
#define MIMI_TG_POLL_TIMEOUT_S 30
#define MIMI_TG_MAX_MSG_LEN 4096
#define MIMI_TG_POLL_STACK (12 * 1024)
#define MIMI_TG_POLL_PRIO 5
#define MIMI_TG_POLL_CORE 0

/* Agent Loop */
#define MIMI_AGENT_STACK (24 * 1024)
#define MIMI_AGENT_PRIO 6
#define MIMI_AGENT_CORE 1
#define MIMI_AGENT_MAX_HISTORY 20
#define MIMI_AGENT_MAX_TOOL_ITER 10
#define MIMI_MAX_TOOL_CALLS 4

/* Skills */
#define MIMI_SKILLS_PREFIX_OLD MIMI_SPIFFS_BASE "/skills/"

/* Timezone (POSIX TZ format) */
#define MIMI_TIMEZONE "PST8PDT,M3.2.0,M11.1.0"

/* LLM */
#define MIMI_LLM_DEFAULT_MODEL "gemini-flash-latest"
#define MIMI_LLM_PROVIDER_DEFAULT "gemini"
#define MIMI_LLM_MAX_TOKENS 4096
#define MIMI_LLM_API_URL "https://api.anthropic.com/v1/messages"
#define MIMI_OPENAI_API_URL "https://api.openai.com/v1/chat/completions"
#define MIMI_LLM_API_VERSION "2023-06-01"
#define MIMI_LLM_STREAM_BUF_SIZE (64 * 1024)

/* Message Bus */
#define MIMI_BUS_QUEUE_LEN 8
#define MIMI_OUTBOUND_STACK (8 * 1024)
#define MIMI_OUTBOUND_PRIO 5
#define MIMI_OUTBOUND_CORE 0

/* Memory / SPIFFS Workspace */
#define MIMI_SPIFFS_BASE       "/spiffs"
#define MIMI_WORKSPACE_DIR     MIMI_SPIFFS_BASE "/workspace"
#define MIMI_SESSION_DIR       MIMI_SPIFFS_BASE "/sessions"

#define MIMI_IDENTITY_FILE     MIMI_WORKSPACE_DIR "/IDENTITY.md"
#define MIMI_SOUL_FILE         MIMI_WORKSPACE_DIR "/SOUL.md"
#define MIMI_AGENT_FILE        MIMI_WORKSPACE_DIR "/AGENT.md"
#define MIMI_USER_FILE         MIMI_WORKSPACE_DIR "/USER.md"
#define MIMI_TOOLS_FILE        MIMI_WORKSPACE_DIR "/TOOLS.md"
#define MIMI_MEMORY_FILE       MIMI_WORKSPACE_DIR "/MEMORY.md"

#define MIMI_SKILLS_PREFIX     MIMI_WORKSPACE_DIR "/skills/"
#define MIMI_HISTORY_PREFIX    MIMI_WORKSPACE_DIR "/history/"

#define MIMI_CONTEXT_BUF_SIZE  (64 * 1024)
#define MIMI_SESSION_MAX_MSGS  20

/* WebSocket Gateway */
#define MIMI_WS_PORT 18789
#define MIMI_WS_MAX_CLIENTS 4

/* Serial CLI */
#define MIMI_CLI_STACK (4 * 1024)
#define MIMI_CLI_PRIO 3
#define MIMI_CLI_CORE 0

/* RGB LED Configuration (WS2812B) */
#define MIMI_RGB_LED_PIN 48 // Default for ESP32-S3 DevKit
#define MIMI_RGB_LED_COUNT 1

/* State Colors (Moods) */
#define MIMI_COLOR_ONLINE    0x00FF00 // Green
#define MIMI_COLOR_THINKING  0x800080 // Purple
#define MIMI_COLOR_EXECUTING 0x0000FF // Blue
#define MIMI_COLOR_CONNECTING 0xFFFF00 // Yellow
#define MIMI_COLOR_ERROR     0xFFA500 // Orange
#define MIMI_COLOR_OFFLINE   0xFF0000 // Red
#define MIMI_COLOR_IDLE      0x002200 // Dim Green

/* NVS Namespaces */
#define MIMI_NVS_WIFI "wifi_config"
#define MIMI_NVS_TG "tg_config"
#define MIMI_NVS_DISCORD "discord_config"
#define MIMI_NVS_LLM "llm_config"
#define MIMI_NVS_PROXY "proxy_config"
#define MIMI_NVS_SEARCH "search_config"

/* NVS Keys */
#define MIMI_NVS_KEY_SSID "ssid"
#define MIMI_NVS_KEY_PASS "password"
#define MIMI_NVS_KEY_TG_TOKEN "bot_token"
#define MIMI_NVS_KEY_DISCORD_TOKEN "discord_token"
#define MIMI_NVS_KEY_API_KEY "api_key"
#define MIMI_NVS_KEY_MODEL "model"
#define MIMI_NVS_KEY_PROVIDER "provider"
#define MIMI_NVS_KEY_PROXY_HOST "host"
#define MIMI_NVS_KEY_PROXY_PORT "port"
