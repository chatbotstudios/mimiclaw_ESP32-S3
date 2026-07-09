#pragma once

#include "esp_err.h"
#include <stdbool.h>

typedef enum {
    PAGE_SPLASH = 0,
    PAGE_WELCOME,
    PAGE_THINKING,
    PAGE_DASHBOARD,
    PAGE_COUNT
} ui_page_t;

#ifdef __cplusplus
extern "C" {
#endif

void ui_manager_init(void);
void ui_manager_switch_page(ui_page_t page);
void ui_manager_next_page(void);
void ui_manager_prev_page(void);

// Expose these to be called by mimi.c / agent / tasks
void ui_splash_show(void);
void ui_welcome_create(void);
void ui_thinking_create(void);
void ui_dashboard_create(void);

// Global UI components
void ui_thinking_update(const char *text);
void ui_dashboard_update(const char *wifi_ssid, const char *ip_addr,
                         float battery_v, int battery_pct, float temp,
                         float humidity, bool is_recording,
                         int unread_msgs, const char *uptime_str,
                         bool is_bluetooth_on);

// Expose screens to manager
extern struct _lv_obj_t *scr_welcome;
extern struct _lv_obj_t *scr_thinking;
extern struct _lv_obj_t *scr_dashboard;

#ifdef __cplusplus
}
#endif
