#ifndef MIMI_LED_H
#define MIMI_LED_H

#include "esp_err.h"
#include <stdbool.h>

typedef enum {
    MIMI_LED_RED = 0,
    MIMI_LED_GREEN
} mimi_led_color_t;

typedef enum {
    LED_STATE_IDLE = 0,
    LED_STATE_CONNECTING,
    LED_STATE_THINKING,
    LED_STATE_TOOL_USE,
    LED_STATE_ERROR
} mimi_led_state_t;

/**
 * Initialize the onboard user LEDs (Red and Green) using LEDC for PWM.
 */
esp_err_t led_init(void);

/**
 * Set the continuous background animation state.
 */
void led_set_state(mimi_led_state_t state);

/**
 * Temporarily interrupt the current state for a Message Received animation (2x Red flashes).
 */
void led_trigger_msg_rx(void);

/**
 * Temporarily interrupt the current state for a Message Sent animation (2x Green flashes).
 */
void led_trigger_msg_tx(void);

/**
 * Start a "processing" animation.
 */
void led_start_processing(void);

/**
 * Stop any running animation and turn the LED back to Idle.
 */
void led_stop_processing(void);

/**
 * Set the RGB LED color directly.
 * @param r, g, b 0-255 color values
 */
void led_set_rgb(uint8_t r, uint8_t g, uint8_t b);

/**
 * Set the RGB LED based on a hex color (e.g., 0xFF00FF)
 */
void led_set_color(uint32_t color_hex);

/**
 * Set the LED based on a system state (e.g. MIMI_COLOR_THINKING)
 * Maps old color constants to new animation states.
 */
void led_set_state_color(uint32_t color_hex);

#endif // MIMI_LED_H
