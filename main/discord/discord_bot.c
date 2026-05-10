#include "discord_bot.h"
#include "mimi_config.h"

#include "esp_log.h"
#include "esp_websocket_client.h"
#include "esp_http_client.h"
#include "nvs.h"
#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_crt_bundle.h"
#include "bus/message_bus.h"

#include <string.h>

static const char *TAG = "discord";

static char s_bot_token[128] = {0};
static char s_bot_id[32] = {0};
static char s_owner_id[32] = {0};
static esp_websocket_client_handle_t s_ws_client = NULL;
static bool s_is_running = false;
static int s_heartbeat_interval = 41250; // Default fallback

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    switch (event_id) {
        case WEBSOCKET_EVENT_CONNECTED:
            ESP_LOGI(TAG, "Connected to Discord Gateway");
            break;
        case WEBSOCKET_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "Disconnected from Discord Gateway");
            break;
        case WEBSOCKET_EVENT_DATA:
            if (data->op_code == 0x01 || data->op_code == 0x00) {
                // Accumulate chunks
                static char *s_ws_buffer = NULL;
                static size_t s_ws_len = 0;

                if (data->data_len > 0) {
                    char *new_buf = heap_caps_realloc(s_ws_buffer, s_ws_len + data->data_len + 1, MALLOC_CAP_SPIRAM);
                    if (!new_buf) {
                        ESP_LOGE(TAG, "OOM parsing Discord event in PSRAM!");
                        heap_caps_free(s_ws_buffer);
                        s_ws_buffer = NULL;
                        s_ws_len = 0;
                        break;
                    }
                    s_ws_buffer = new_buf;
                    memcpy(s_ws_buffer + s_ws_len, data->data_ptr, data->data_len);
                    s_ws_len += data->data_len;
                    s_ws_buffer[s_ws_len] = '\0';
                }

                // If this is the final fragment (or not fragmented)
                if (data->payload_len == data->payload_offset + data->data_len) {
                    cJSON *root = cJSON_Parse(s_ws_buffer);
                    heap_caps_free(s_ws_buffer);
                    s_ws_buffer = NULL;
                    s_ws_len = 0;

                    if (root) {
                    cJSON *op = cJSON_GetObjectItem(root, "op");
                    cJSON *t = cJSON_GetObjectItem(root, "t");
                    cJSON *d = cJSON_GetObjectItem(root, "d");

                    if (op && op->valueint == 10) { // Hello
                        cJSON *hb = cJSON_GetObjectItem(d, "heartbeat_interval");
                        if (hb) {
                            s_heartbeat_interval = hb->valueint;
                            ESP_LOGI(TAG, "Heartbeat interval set to %d ms", s_heartbeat_interval);

                            char identify_payload[512];
                            snprintf(identify_payload, sizeof(identify_payload),
                                "{\"op\":2,\"d\":{\"token\":\"%s\",\"intents\":37377,\"properties\":{\"os\":\"FreeRTOS\",\"browser\":\"MimiClaw\",\"device\":\"ESP32-S3\"}}}",
                                s_bot_token);
                            
                            esp_websocket_client_send_text(s_ws_client, identify_payload, strlen(identify_payload), portMAX_DELAY);
                            ESP_LOGI(TAG, "Sent Identify Payload to Gateway");
                        }
                    } else if (op && op->valueint == 0) { // Dispatch Event
                        if (t && t->valuestring) {
                            if (strcmp(t->valuestring, "READY") == 0) {
                                cJSON *user = cJSON_GetObjectItem(d, "user");
                                if (user) {
                                    cJSON *id = cJSON_GetObjectItem(user, "id");
                                    if (id && id->valuestring) {
                                        strncpy(s_bot_id, id->valuestring, sizeof(s_bot_id) - 1);
                                    }
                                }
                                ESP_LOGI(TAG, "Discord Bot is READY! Logged in successfully. Bot ID: %s", s_bot_id);
                            } else if (strcmp(t->valuestring, "MESSAGE_CREATE") == 0) {
                                cJSON *author = cJSON_GetObjectItem(d, "author");
                                cJSON *bot_flag = author ? cJSON_GetObjectItem(author, "bot") : NULL;
                                
                                // Ignore messages from bots (or ourselves)
                                if (bot_flag && cJSON_IsTrue(bot_flag)) {
                                    cJSON_Delete(root);
                                    break;
                                }

                                cJSON *content = cJSON_GetObjectItem(d, "content");
                                cJSON *channel_id = cJSON_GetObjectItem(d, "channel_id");
                                cJSON *author_id = author ? cJSON_GetObjectItem(author, "id") : NULL;

                                if (author_id && author_id->valuestring && s_owner_id[0] != '\0') {
                                    if (strcmp(author_id->valuestring, s_owner_id) == 0) {
                                        ESP_LOGI(TAG, "Message received from Owner (%s)", s_owner_id);
                                    }
                                }

                                if (content && content->valuestring && channel_id && channel_id->valuestring) {
                                    // Check if the bot was mentioned OR just reply to anything for now
                                    // Let's look for our bot ID mention: <@BOT_ID>
                                    char mention_str[64];
                                    snprintf(mention_str, sizeof(mention_str), "<@%s>", s_bot_id);
                                    
                                    if (strstr(content->valuestring, mention_str) != NULL || strcasestr(content->valuestring, "mimi") != NULL) {
                                        ESP_LOGI(TAG, "Message for Mimi: %s", content->valuestring);
                                        
                                        // Send to Message Bus
                                        mimi_msg_t msg;
                                        strncpy(msg.channel, "discord", sizeof(msg.channel) - 1);
                                        strncpy(msg.chat_id, channel_id->valuestring, sizeof(msg.chat_id) - 1);
                                        msg.content = strdup(content->valuestring);
                                        
                                        // Need to include message_bus.h at the top of file!
                                        // I'll assume message_bus_push_inbound is declared
                                        extern esp_err_t message_bus_push_inbound(const mimi_msg_t *msg);
                                        message_bus_push_inbound(&msg);
                                    }
                                }
                            }
                        }
                    }
                    cJSON_Delete(root);
                }
                }
            }
            break;
        case WEBSOCKET_EVENT_ERROR:
            ESP_LOGE(TAG, "WebSocket Error");
            break;
    }
}

static void discord_heartbeat_task(void *pvParameters) {
    while (s_is_running) {
        vTaskDelay(pdMS_TO_TICKS(s_heartbeat_interval));
        if (s_ws_client && esp_websocket_client_is_connected(s_ws_client)) {
            // Discord Heartbeat payload (Opcode 1)
            const char *hb_payload = "{\"op\":1,\"d\":null}";
            esp_websocket_client_send_text(s_ws_client, hb_payload, strlen(hb_payload), portMAX_DELAY);
            ESP_LOGD(TAG, "Sent Heartbeat");
        }
    }
    vTaskDelete(NULL);
}

esp_err_t discord_bot_init(void) {
    nvs_handle_t nvs;
    if (nvs_open(MIMI_NVS_DISCORD, NVS_READONLY, &nvs) == ESP_OK) {
        size_t len = sizeof(s_bot_token);
        nvs_get_str(nvs, MIMI_NVS_KEY_DISCORD_TOKEN, s_bot_token, &len);
        nvs_close(nvs);
    }

    if (s_bot_token[0] == '\0') {
        if (strlen(MIMI_SECRET_DISCORD_TOKEN) > 0) {
            strncpy(s_bot_token, MIMI_SECRET_DISCORD_TOKEN, sizeof(s_bot_token) - 1);
        } else {
            ESP_LOGW(TAG, "No Discord token found in NVS or hardcoded. Use CLI to configure.");
            return ESP_ERR_NOT_FOUND;
        }
    }

    /* Initialize Owner ID */
    if (nvs_open(MIMI_NVS_DISCORD, NVS_READONLY, &nvs) == ESP_OK) {
        size_t len = sizeof(s_owner_id);
        nvs_get_str(nvs, "owner_id", s_owner_id, &len);
        nvs_close(nvs);
    }

    if (s_owner_id[0] == '\0' && strlen(MIMI_SECRET_DISCORD_USER_ID) > 0) {
        strncpy(s_owner_id, MIMI_SECRET_DISCORD_USER_ID, sizeof(s_owner_id) - 1);
    }

    if (s_owner_id[0] != '\0') {
        ESP_LOGI(TAG, "Discord Owner ID configured: %s", s_owner_id);
    }

    ESP_LOGI(TAG, "Discord Bot initialized");
    return ESP_OK;
}

esp_err_t discord_bot_start(void) {
    if (s_bot_token[0] == '\0') {
        return ESP_ERR_INVALID_STATE;
    }

    if (s_is_running) {
        return ESP_OK; // Already running
    }

    const esp_websocket_client_config_t ws_cfg = {
        .uri = "wss://gateway.discord.gg/?v=10&encoding=json",
        .buffer_size = 16 * 1024, // Discord READY payload is large
        // .crt_bundle_attach = esp_crt_bundle_attach, // Disabled to use global insecure flags
    };

    s_ws_client = esp_websocket_client_init(&ws_cfg);
    if (!s_ws_client) {
        ESP_LOGE(TAG, "Failed to initialize WebSocket client");
        return ESP_FAIL;
    }

    esp_websocket_register_events(s_ws_client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)s_ws_client);

    esp_err_t err = esp_websocket_client_start(s_ws_client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start WebSocket client");
        return err;
    }

    s_is_running = true;

    xTaskCreatePinnedToCore(discord_heartbeat_task, "discord_hb", 4096, NULL, 5, NULL, 0);

    ESP_LOGI(TAG, "Discord Bot started");
    return ESP_OK;
}

#include "esp_http_client.h"

esp_err_t discord_bot_send_message(const char *channel_id, const char *text) {
    if (!channel_id || !text || s_bot_token[0] == '\0') {
        return ESP_ERR_INVALID_ARG;
    }

    char url[256];
    snprintf(url, sizeof(url), "https://discord.com/api/v10/channels/%s/messages", channel_id);

    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "content", text);
    char *post_data = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    if (!post_data) return ESP_FAIL;

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        // .crt_bundle_attach = esp_crt_bundle_attach,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) {
        free(post_data);
        return ESP_FAIL;
    }

    char auth_header[256];
    snprintf(auth_header, sizeof(auth_header), "Bot %s", s_bot_token);

    esp_http_client_set_header(client, "Authorization", auth_header);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        if (status == 200) {
            ESP_LOGI(TAG, "Message sent successfully to %s", channel_id);
        } else {
            ESP_LOGW(TAG, "Failed to send message, HTTP status: %d", status);
        }
    } else {
        ESP_LOGE(TAG, "HTTP POST failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    free(post_data);

    return err;
}

esp_err_t discord_bot_send_typing(const char *channel_id) {
    if (!channel_id || s_bot_token[0] == '\0') {
        return ESP_ERR_INVALID_ARG;
    }

    char url[256];
    snprintf(url, sizeof(url), "https://discord.com/api/v10/channels/%s/typing", channel_id);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        // .crt_bundle_attach = esp_crt_bundle_attach,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) return ESP_FAIL;

    char auth_header[256];
    snprintf(auth_header, sizeof(auth_header), "Bot %s", s_bot_token);
    esp_http_client_set_header(client, "Authorization", auth_header);
    
    /* Content-Length must be 0 for this POST */
    esp_http_client_set_header(client, "Content-Length", "0");

    esp_err_t err = esp_http_client_perform(client);
    esp_http_client_cleanup(client);

    return err;
}

bool discord_bot_is_connected(void) {
    return (s_ws_client && esp_websocket_client_is_connected(s_ws_client));
}
