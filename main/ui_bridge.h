#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Initialize the C++ UI engine
void ui_bridge_init(void);

// Update the global state and invalidate views
void ui_bridge_update(const char *wifi_ssid, const char *ip_addr,
                      float battery_v, int battery_pct, float temp,
                      float humidity, bool is_bluetooth_on,
                      int pm_mode, const char *uptime_str,
                      bool thinking);

// Advance the carousel
void ui_bridge_next_page(void);

#ifdef __cplusplus
}
#endif
