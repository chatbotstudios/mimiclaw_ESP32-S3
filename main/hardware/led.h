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

#endif // MIMI_LED_H
