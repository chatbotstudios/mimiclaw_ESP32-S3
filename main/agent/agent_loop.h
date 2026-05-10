#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * Initialize the agent loop.
 */
esp_err_t agent_loop_init(void);

/**
 * Start the agent loop task (runs on Core 1).
 * Consumes from inbound queue, calls Claude API, pushes to outbound queue.
 */
esp_err_t agent_loop_start(void);

/**
 * Get the agent task handle for diagnostics.
 */
TaskHandle_t agent_loop_get_task_handle(void);
