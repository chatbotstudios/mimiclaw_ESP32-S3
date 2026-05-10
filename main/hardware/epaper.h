#pragma once

#include "esp_err.h"
#include <stdint.h>

/**
 * Initialize the 1.54" ePaper display.
 */
esp_err_t epaper_init(void);

/**
 * Clear the screen (white).
 */
void epaper_clear(void);

/**
 * Draw a simple text message on the screen.
 * (Uses a built-in basic font for now).
 */
void epaper_draw_text(const char *text);

/**
 * Draw text with advanced styling and positioning.
 * @param text The string to draw
 * @param start_x X coordinate (0-199)
 * @param start_y Y coordinate (0-199)
 * @param scale Font multiplier
 * @param invert White text on black background
 */
void epaper_draw_text_ext(const char *text, int start_x, int start_y, int scale, bool invert);

/**
 * Draw a 16x16 icon bitmap.
 * @param start_x X coordinate (0-199)
 * @param start_y Y coordinate (0-199)
 * @param icon The 32-byte 16x16 bitmap array
 * @param strike If true, draws a diagonal slash over the icon (for OFF states)
 */
void epaper_draw_icon(int start_x, int start_y, const uint8_t *icon, bool strike);

/**
 * Update the display with current framebuffer contents.
 */
void epaper_update_screen(void);

/**
 * Get pointer to the raw framebuffer (200x200 pixels, 5000 bytes).
 */
uint8_t *epaper_get_buffer(void);

/**
 * Show a dashboard with system metrics.
 */
void epaper_show_dashboard(const char *ssid, const char *ip, float voltage, int batt_pct, float temp, float hum, bool bt_on, int pwr_mode, const char *uptime_str, bool thinking);

/**
 * Trigger a full hardware refresh (eliminates ghosting).
 */
void epaper_full_refresh(void);

/**
 * Set inversion mode (true = white on black).
 */
void epaper_set_invert(bool invert);

/**
 * Put the display into deep sleep to save power.
 */
void epaper_sleep(void);
