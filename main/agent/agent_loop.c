#include "agent_loop.h"
#include "agent/context_builder.h"
#include "mimi_config.h"
#include "bus/message_bus.h"
#include "llm/llm_proxy.h"
#include "memory/session_mgr.h"
#include "tools/tool_registry.h"
#include "hardware/rules_engine.h"
#include "hardware/pm_system.h"
#include "hardware/led.h"
#include "mimi.h"

#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_random.h"
#include "esp_console.h"
#include "cJSON.h"
#include "agent/agent_metrics.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "agent";

#define TOOL_OUTPUT_SIZE  (8 * 1024)

static TaskHandle_t s_agent_task_handle = NULL;

/* Build the assistant content array from llm_response_t for the messages history.
 * Returns a cJSON array with text and tool_use blocks. */
static cJSON *build_assistant_content(const llm_response_t *resp)
{
    cJSON *content = cJSON_CreateArray();

    /* Text block */
    if (resp->text && resp->text_len > 0) {
        cJSON *text_block = cJSON_CreateObject();
        cJSON_AddStringToObject(text_block, "type", "text");
        cJSON_AddStringToObject(text_block, "text", resp->text);
        cJSON_AddItemToArray(content, text_block);
    }

    /* Tool use blocks */
    for (int i = 0; i < resp->call_count; i++) {
        const llm_tool_call_t *call = &resp->calls[i];
        cJSON *tool_block = cJSON_CreateObject();
        cJSON_AddStringToObject(tool_block, "type", "tool_use");
        cJSON_AddStringToObject(tool_block, "id", call->id);
        cJSON_AddStringToObject(tool_block, "name", call->name);

        cJSON *input = cJSON_Parse(call->input);
        if (input) {
            cJSON_AddItemToObject(tool_block, "input", input);
        } else {
            cJSON_AddItemToObject(tool_block, "input", cJSON_CreateObject());
        }

        if (call->thought_signature) {
            cJSON_AddStringToObject(tool_block, "thought_signature", call->thought_signature);
        }

        cJSON_AddItemToArray(content, tool_block);
    }

    return content;
}

/* Build the user message with tool_result blocks */
static cJSON *build_tool_results(const llm_response_t *resp, char *tool_output, size_t tool_output_size)
{
    cJSON *content = cJSON_CreateArray();

    for (int i = 0; i < resp->call_count; i++) {
        const llm_tool_call_t *call = &resp->calls[i];

        /* Execute tool */
        tool_output[0] = '\0';
        tool_registry_execute(call->name, call->input, tool_output, tool_output_size);

        ESP_LOGI(TAG, "Tool %s result: %d bytes", call->name, (int)strlen(tool_output));

        /* Build tool_result block */
        cJSON *result_block = cJSON_CreateObject();
        cJSON_AddStringToObject(result_block, "type", "tool_result");
        cJSON_AddStringToObject(result_block, "tool_use_id", call->id);
        cJSON_AddStringToObject(result_block, "name", call->name);
        cJSON_AddStringToObject(result_block, "content", tool_output);
        cJSON_AddItemToArray(content, result_block);
    }

    return content;
}

static void agent_loop_task(void *arg)
{
    ESP_LOGI(TAG, "Agent loop started on core %d", xPortGetCoreID());

    /* Allocate large buffers from PSRAM */
    char *system_prompt = heap_caps_calloc(1, MIMI_CONTEXT_BUF_SIZE, MALLOC_CAP_SPIRAM);
    char *history_json = heap_caps_calloc(1, MIMI_LLM_STREAM_BUF_SIZE, MALLOC_CAP_SPIRAM);
    char *tool_output = heap_caps_calloc(1, TOOL_OUTPUT_SIZE, MALLOC_CAP_SPIRAM);

    if (!system_prompt || !history_json || !tool_output) {
        ESP_LOGE(TAG, "Failed to allocate PSRAM buffers");
        vTaskDelete(NULL);
        return;
    }

    const char *tools_json = tool_registry_get_tools_json();

    while (1) {
        /* Evaluate Local Rules (Subconscious) Every Second */
        rules_engine_evaluate();

        mimi_msg_t msg;
        esp_err_t err = message_bus_pop_inbound(&msg, pdMS_TO_TICKS(1000));
        if (err != ESP_OK) continue;

        ESP_LOGI(TAG, "Processing message from %s:%s", msg.channel, msg.chat_id);
        led_start_processing();
        mimi_update_dashboard(true);

        /* Intercept CLI commands (start with '/') */
        if (msg.content[0] == '/') {
            ESP_LOGI(TAG, "Intercepted CLI command: %s", msg.content + 1);
            
            char *capture_buf = NULL;
            size_t capture_size = 0;
            FILE *mem_file = open_memstream(&capture_buf, &capture_size);
            if (mem_file) {
                FILE *orig_stdout = stdout;
                stdout = mem_file;
                
                int ret_code = 0;
                esp_err_t err = esp_console_run(msg.content + 1, &ret_code);
                
                fflush(stdout);
                stdout = orig_stdout;
                fclose(mem_file);
                
                mimi_msg_t out = {0};
                strncpy(out.channel, msg.channel, sizeof(out.channel) - 1);
                strncpy(out.chat_id, msg.chat_id, sizeof(out.chat_id) - 1);
                
                if (err == ESP_ERR_NOT_FOUND) {
                    out.content = strdup("Command not found. Type /help for a list of commands.");
                } else if (err == ESP_OK && capture_buf && capture_size > 0) {
                    // Prepend and append code block formatting for Discord/TG
                    char *formatted = malloc(capture_size + 16);
                    if (formatted) {
                        snprintf(formatted, capture_size + 16, "```\n%s\n```", capture_buf);
                        out.content = formatted;
                    } else {
                        out.content = strdup(capture_buf);
                    }
                } else if (err != ESP_OK) {
                    char err_str[128];
                    snprintf(err_str, sizeof(err_str), "Command error: %s", esp_err_to_name(err));
                    out.content = strdup(err_str);
                } else {
                    out.content = strdup("Command executed successfully (no output).");
                }
                
                if (out.content) {
                    message_bus_push_outbound(&out);
                }
                free(capture_buf);
            }
            free(msg.content);
            led_stop_processing();
            mimi_update_dashboard(false);
            continue; /* Skip the LLM completely */
        }

        /* 1. Get cached system prompt from PSRAM */
        context_get_cached_prompt(system_prompt, MIMI_CONTEXT_BUF_SIZE);

        /* 2. Load session history into cJSON array */
        session_get_history_json(msg.channel, msg.chat_id, history_json,
                                 MIMI_LLM_STREAM_BUF_SIZE, MIMI_AGENT_MAX_HISTORY);

        cJSON *messages = cJSON_Parse(history_json);
        if (!messages) messages = cJSON_CreateArray();

        /* 3. Append current user message */
        cJSON *user_msg = cJSON_CreateObject();
        cJSON_AddStringToObject(user_msg, "role", "user");
        cJSON_AddStringToObject(user_msg, "content", msg.content);
        cJSON_AddItemToArray(messages, user_msg);

        /* Start processing (Thinking = Purple) */
        led_start_processing();
        mimi_update_dashboard(true);

        /* 4. ReAct loop */
        char *final_text = NULL;
        int tool_iter = 0;
        llm_msg_type_t msg_type = LLM_MSG_TOOL_CALL;

        while (msg_type == LLM_MSG_TOOL_CALL && tool_iter < MIMI_AGENT_MAX_TOOL_ITER) {
            /* Set Executing color (Blue) */
            led_set_state_color(MIMI_COLOR_EXECUTING);
            
            tool_iter++;
            /* Trigger 'Typing...' indicator on the channel */
            message_bus_send_typing(msg.channel, msg.chat_id);

            llm_response_t resp;
            err = llm_chat_tools(system_prompt, messages, tools_json, &resp);

            if (err != ESP_OK) {
                ESP_LOGE(TAG, "LLM call failed: %s", esp_err_to_name(err));
                break;
            }

            if (!resp.tool_use) {
                /* Normal completion — save final text and break */
                if (resp.text && resp.text_len > 0) {
                    final_text = strdup(resp.text);
                }
                msg_type = LLM_MSG_TEXT;
                llm_response_free(&resp);
                break;
            }

            ESP_LOGI(TAG, "Tool use iteration %d: %d calls", tool_iter, resp.call_count);

            /* Append assistant message with content array */
            cJSON *asst_msg = cJSON_CreateObject();
            cJSON_AddStringToObject(asst_msg, "role", "assistant");
            cJSON_AddItemToObject(asst_msg, "content", build_assistant_content(&resp));
            cJSON_AddItemToArray(messages, asst_msg);

            /* Return to Thinking color (Purple) for next iteration */
            led_set_state_color(MIMI_COLOR_THINKING);

            /* Execute tools and append results */
            cJSON *tool_results = build_tool_results(&resp, tool_output, TOOL_OUTPUT_SIZE);
            cJSON *result_msg = cJSON_CreateObject();
            cJSON_AddStringToObject(result_msg, "role", "user");
            cJSON_AddItemToObject(result_msg, "content", tool_results);
            cJSON_AddItemToArray(messages, result_msg);

            llm_response_free(&resp);
        }

        cJSON_Delete(messages);

        /* 5. Send response */
        if (final_text && final_text[0]) {
            /* Save to session (only user text + final assistant text) */
            session_append(msg.channel, msg.chat_id, "user", msg.content);
            session_append(msg.channel, msg.chat_id, "assistant", final_text);

            /* Push response to outbound */
            mimi_msg_t out = {0};
            strncpy(out.channel, msg.channel, sizeof(out.channel) - 1);
            strncpy(out.chat_id, msg.chat_id, sizeof(out.chat_id) - 1);
            out.content = final_text;  /* transfer ownership */
            message_bus_push_outbound(&out);
        } else {
            /* Error or empty response */
            free(final_text);
            mimi_msg_t out = {0};
            strncpy(out.channel, msg.channel, sizeof(out.channel) - 1);
            strncpy(out.chat_id, msg.chat_id, sizeof(out.chat_id) - 1);
            out.content = strdup("Sorry, I encountered an error.");
            if (out.content) {
                message_bus_push_outbound(&out);
            }
        }
        led_stop_processing(); // Back to Green
        mimi_update_dashboard(false);
        led_set_state_color(MIMI_COLOR_ONLINE); // Ensure Online color is solid

        /* Free inbound message content */
        free(msg.content);

        /* Log memory status */
        ESP_LOGI(TAG, "Free PSRAM: %d bytes",
                 (int)heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    }
}

esp_err_t agent_loop_init(void)
{
    ESP_LOGI(TAG, "Agent loop initialized");
    return ESP_OK;
}

esp_err_t agent_loop_start(void)
{
    BaseType_t ret = xTaskCreatePinnedToCore(
        agent_loop_task, "agent_loop",
        MIMI_AGENT_STACK, NULL,
        MIMI_AGENT_PRIO, &s_agent_task_handle, MIMI_AGENT_CORE);

    return (ret == pdPASS) ? ESP_OK : ESP_FAIL;
}

TaskHandle_t agent_loop_get_task_handle(void)
{
    return s_agent_task_handle;
}
