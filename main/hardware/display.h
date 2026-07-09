#pragma once

#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the display hardware (ePaper or AMOLED).
 */
esp_err_t mimi_display_init(void);

/**
 * @brief Show the startup animation on the display.
 */
void mimi_display_show_startup_animation(void);

/**
 * @brief Show the system dashboard on the display.
 * 
 * @param wifi_ssid Connected WiFi SSID
 * @param ip_addr Local IP Address
 * @param batt_v Battery Voltage
 * @param batt_pct Battery Percentage
 * @param temp Temperature (Celsius)
 * @param hum Humidity (%)
 * @param bt_on True if Bluetooth is enabled
 * @param pwr_mode Power mode integer (0=ultra, 1=balanced, 2=perf)
 * @param uptime String representing system uptime
 * @param thinking True if the agent is actively thinking/generating
 */
void mimi_display_show_dashboard(const char *wifi_ssid, const char *ip_addr, 
                                 float batt_v, int batt_pct, 
                                 float temp, float hum, 
                                 bool bt_on, int pwr_mode, 
                                 const char *uptime, bool thinking);

/**
 * @brief Draw custom UI or text (used by the AI tool)
 * 
 * @param text The text to draw
 * @param x X coordinate
 * @param y Y coordinate
 * @param font_size Font scale
 * @param color Color hex code (e.g. 0xFFFFFF for white)
 */
void mimi_display_draw_text(const char *text, int x, int y, int font_size, uint32_t color);

/**
 * @brief Flush any pending drawing operations to the screen
 */
void mimi_display_flush(void);

#ifdef __cplusplus
}
#endif
