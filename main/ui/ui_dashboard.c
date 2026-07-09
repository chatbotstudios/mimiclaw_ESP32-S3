#include "ui_manager.h"
#include "font_manager.h"
#include "lvgl.h"

#ifdef CONFIG_BOARD_AMOLED_175
#include "bsp/esp-bsp.h"
#endif

void ui_dashboard_create(void) {
    scr_dashboard = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_dashboard, lv_color_hex(0x000000), 0);

    lv_obj_t *msg = lv_label_create(scr_dashboard);
    lv_label_set_text(msg, "SCREEN 3");
    lv_obj_set_style_text_font(msg, font_manager_get_core_font_48(), 0);
    lv_obj_set_style_text_color(msg, lv_color_hex(0x4285F4), 0);
    lv_obj_align(msg, LV_ALIGN_CENTER, 0, 0);
}

void ui_dashboard_update(const char *wifi_ssid, const char *ip_addr,
                         float battery_v, int battery_pct, float temp,
                         float humidity, bool is_recording,
                         int unread_msgs, const char *uptime_str,
                         bool is_bluetooth_on) {
    // Empty for now as it's just a placeholder
}
