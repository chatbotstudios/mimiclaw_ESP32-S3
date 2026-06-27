#include "hardware/led.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mimi_config.h"
#include <math.h>

#define LED_PIN_RED 3
#define LED_PIN_GREEN 1

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL_RED        LEDC_CHANNEL_0
#define LEDC_CHANNEL_GREEN      LEDC_CHANNEL_1
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT
#define LEDC_FREQUENCY          (5000)

static const char *TAG = "led";

static mimi_led_state_t s_current_state = LED_STATE_IDLE;
static bool s_override_active = false;
static int s_override_type = 0; // 1 = RX, 2 = TX

static void ledc_init_pin(int gpio_num, ledc_channel_t channel) {
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = channel,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = gpio_num,
        .duty           = 0,
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);
}

static void set_duty(ledc_channel_t channel, uint32_t duty) {
    ledc_set_duty(LEDC_MODE, channel, duty);
    ledc_update_duty(LEDC_MODE, channel);
}

static void led_task(void *arg) {
    uint32_t max_duty = (1 << 13) - 1;
    float phase = 0.0f;

    while (1) {
        if (s_override_active) {
            // Flash animation blocks here
            if (s_override_type == 1) { // RX: 2 quick Red flashes
                set_duty(LEDC_CHANNEL_GREEN, 0);
                set_duty(LEDC_CHANNEL_RED, max_duty); vTaskDelay(pdMS_TO_TICKS(100));
                set_duty(LEDC_CHANNEL_RED, 0); vTaskDelay(pdMS_TO_TICKS(100));
                set_duty(LEDC_CHANNEL_RED, max_duty); vTaskDelay(pdMS_TO_TICKS(100));
                set_duty(LEDC_CHANNEL_RED, 0);
            } else if (s_override_type == 2) { // TX: 2 quick Green flashes
                set_duty(LEDC_CHANNEL_RED, 0);
                set_duty(LEDC_CHANNEL_GREEN, max_duty); vTaskDelay(pdMS_TO_TICKS(100));
                set_duty(LEDC_CHANNEL_GREEN, 0); vTaskDelay(pdMS_TO_TICKS(100));
                set_duty(LEDC_CHANNEL_GREEN, max_duty); vTaskDelay(pdMS_TO_TICKS(100));
                set_duty(LEDC_CHANNEL_GREEN, 0);
            }
            s_override_active = false;
            phase = 0; // Reset phase for smooth return to breathe
        } else {
            // Handle continuous states
            switch (s_current_state) {
                case LED_STATE_IDLE:
                    set_duty(LEDC_CHANNEL_RED, 0);
                    set_duty(LEDC_CHANNEL_GREEN, max_duty); // Solid Green
                    vTaskDelay(pdMS_TO_TICKS(100));
                    break;

                case LED_STATE_CONNECTING:
                    // 0.5s Pulsing Yellow (Red + Green on/off)
                    set_duty(LEDC_CHANNEL_RED, max_duty);
                    set_duty(LEDC_CHANNEL_GREEN, max_duty);
                    vTaskDelay(pdMS_TO_TICKS(500));
                    if(s_override_active || s_current_state != LED_STATE_CONNECTING) break;
                    set_duty(LEDC_CHANNEL_RED, 0);
                    set_duty(LEDC_CHANNEL_GREEN, 0);
                    vTaskDelay(pdMS_TO_TICKS(500));
                    break;

                case LED_STATE_THINKING:
                {
                    // Breathe Green
                    float val = (sin(phase) + 1.0) / 2.0; // 0.0 to 1.0
                    uint32_t duty = (uint32_t)(val * max_duty);
                    set_duty(LEDC_CHANNEL_RED, 0);
                    set_duty(LEDC_CHANNEL_GREEN, duty);
                    phase += 0.1f;
                    vTaskDelay(pdMS_TO_TICKS(30));
                    break;
                }

                case LED_STATE_TOOL_USE:
                    // 0.5s Pulsing Green
                    set_duty(LEDC_CHANNEL_RED, 0);
                    set_duty(LEDC_CHANNEL_GREEN, max_duty);
                    vTaskDelay(pdMS_TO_TICKS(500));
                    if(s_override_active || s_current_state != LED_STATE_TOOL_USE) break;
                    set_duty(LEDC_CHANNEL_GREEN, 0);
                    vTaskDelay(pdMS_TO_TICKS(500));
                    break;

                case LED_STATE_ERROR:
                {
                    // Breathe Red
                    float val = (sin(phase) + 1.0) / 2.0;
                    uint32_t duty = (uint32_t)(val * max_duty);
                    set_duty(LEDC_CHANNEL_GREEN, 0);
                    set_duty(LEDC_CHANNEL_RED, duty);
                    phase += 0.1f;
                    vTaskDelay(pdMS_TO_TICKS(30));
                    break;
                }
            }
        }
    }
}

esp_err_t led_init(void) {
    ledc_init_pin(LED_PIN_RED, LEDC_CHANNEL_RED);
    ledc_init_pin(LED_PIN_GREEN, LEDC_CHANNEL_GREEN);

    xTaskCreate(led_task, "led_task", 4096, NULL, 5, NULL);
    return ESP_OK;
}

void led_set_state(mimi_led_state_t state) {
    s_current_state = state;
}

void led_trigger_msg_rx(void) {
    s_override_type = 1;
    s_override_active = true;
}

void led_trigger_msg_tx(void) {
    s_override_type = 2;
    s_override_active = true;
}

void led_start_processing(void) {
    led_set_state(LED_STATE_THINKING);
}

void led_stop_processing(void) {
    led_set_state(LED_STATE_IDLE);
}

void led_set_state_color(uint32_t color_hex) {
    if (color_hex == MIMI_COLOR_ONLINE) {
        led_set_state(LED_STATE_IDLE);
    } else if (color_hex == MIMI_COLOR_CONNECTING) {
        led_set_state(LED_STATE_CONNECTING);
    } else if (color_hex == MIMI_COLOR_THINKING) {
        led_set_state(LED_STATE_THINKING);
    } else {
        led_set_state(LED_STATE_ERROR);
    }
}

// Stubs for RGB backwards compatibility
void led_set_rgb(uint8_t r, uint8_t g, uint8_t b) {}
void led_set_color(uint32_t color_hex) {}
