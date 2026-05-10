#ifndef MIMI_LED_H
#define MIMI_LED_H

#include "esp_err.h"
#include <stdbool.h>

typedef enum {
    MIMI_LED_RED = 0,
    MIMI_LED_GREEN
} mimi_led_color_t;

/**
 * Initialize the onboard user LEDs (Red and Green).
 */
esp_err_t led_init(void);

/**
 * Set a specific LED state directly.
 */
void led_set_level(mimi_led_color_t color, int level);

/**
 * Get the current state of a specific LED.
 */
bool led_get_state(mimi_led_color_t color);

/**
 * Start a "processing" animation on the Red LED.
 */
void led_start_processing(void);

/**
 * Stop any running animation and turn the Red LED off.
 */
void led_stop_processing(void);

/**
 * Blink a specific LED.
 */
void led_blink(mimi_led_color_t color, int ms);

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
 */
void led_set_state_color(uint32_t color_hex);

#endif // MIMI_LED_H
