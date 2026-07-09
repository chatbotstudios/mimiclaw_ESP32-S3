#include "ui_manager.h"
#include "font_manager.h"
#include "ui_styles.h"
#include "lvgl.h"
#include <stdio.h>

#ifdef CONFIG_BOARD_AMOLED_175
#include "bsp/esp-bsp.h"
#endif

// UI Element Pointers
static lv_obj_t *lbl_top_status = NULL;
static lv_obj_t *lbl_temp_hum = NULL;

static lv_obj_t *lbl_network = NULL;
static lv_obj_t *lbl_uptime = NULL;

static lv_style_t style_panel;

static lv_obj_t* create_panel(lv_obj_t *parent) {
    lv_obj_t *panel = lv_obj_create(parent);
    lv_obj_add_style(panel, &style_panel, 0);
    lv_obj_set_layout(panel, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(panel, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    return panel;
}

void ui_dashboard_create(void) {
    // Initialize shared styles
    beacon_style_panel_init(&style_panel);

    scr_dashboard = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_dashboard, BEACON_COLOR_BG, 0);

    // Main Flex Container
    lv_obj_t *main_cont = lv_obj_create(scr_dashboard);
    lv_obj_remove_style_all(main_cont);
    lv_obj_set_size(main_cont, lv_pct(100), lv_pct(100));
    lv_obj_set_layout(main_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(main_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(main_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(main_cont, BEACON_GAP_LARGE, 0);

    // --- TOP STATUS BAR ---
    lbl_top_status = lv_label_create(main_cont);
    lv_label_set_text(lbl_top_status, "\xF0\x9F\x93\xB6 WiFi OFF | \xF0\x9F\x94\x8B 0%"); // Emoji placeholders
    lv_obj_set_style_text_font(lbl_top_status, font_manager_get_emoji_font(24), 0);
    lv_obj_set_style_text_color(lbl_top_status, BEACON_COLOR_TEXT_SEC, 0);

    // --- CENTER HUGE DISPLAY (Temp/Hum) ---
    lv_obj_t *center_panel = create_panel(main_cont);
    lv_obj_set_width(center_panel, 300); // Fit within 466px round screen
    
    lbl_temp_hum = lv_label_create(center_panel);
    lv_label_set_text(lbl_temp_hum, "--.-°C  --%");
    lv_obj_set_style_text_font(lbl_temp_hum, font_manager_get_core_font_48(), 0);
    lv_obj_set_style_text_color(lbl_temp_hum, BEACON_COLOR_TEXT_PRM, 0);

    // --- BOTTOM GRID ---
    lv_obj_t *grid = lv_obj_create(main_cont);
    lv_obj_remove_style_all(grid);
    lv_obj_set_layout(grid, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(grid, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(grid, BEACON_GAP_LARGE, 0);

    // Network Panel
    lv_obj_t *pnl_net = create_panel(grid);
    lv_obj_set_size(pnl_net, 160, 100);
    lv_obj_t *lbl_net_title = lv_label_create(pnl_net);
    lv_label_set_text(lbl_net_title, "NETWORK");
    lv_obj_set_style_text_font(lbl_net_title, font_manager_get_core_font_24(), 0);
    lv_obj_set_style_text_color(lbl_net_title, BEACON_COLOR_ACCENT, 0);
    lbl_network = lv_label_create(pnl_net);
    lv_label_set_text(lbl_network, "0.0.0.0");
    lv_obj_set_style_text_font(lbl_network, font_manager_get_core_font_24(), 0);
    lv_obj_set_style_text_color(lbl_network, BEACON_COLOR_TEXT_PRM, 0);

    // System Panel
    lv_obj_t *pnl_sys = create_panel(grid);
    lv_obj_set_size(pnl_sys, 160, 100);
    lv_obj_t *lbl_sys_title = lv_label_create(pnl_sys);
    lv_label_set_text(lbl_sys_title, "UPTIME");
    lv_obj_set_style_text_font(lbl_sys_title, font_manager_get_core_font_24(), 0);
    lv_obj_set_style_text_color(lbl_sys_title, BEACON_COLOR_WARN, 0);
    lbl_uptime = lv_label_create(pnl_sys);
    lv_label_set_text(lbl_uptime, "00:00:00");
    lv_obj_set_style_text_font(lbl_uptime, font_manager_get_core_font_24(), 0);
    lv_obj_set_style_text_color(lbl_uptime, BEACON_COLOR_TEXT_PRM, 0);
}

void ui_dashboard_update(const char *wifi_ssid, const char *ip_addr,
                         float battery_v, int battery_pct, float temp,
                         float humidity, bool is_recording,
                         int unread_msgs, const char *uptime_str,
                         bool is_bluetooth_on) {
                             
    if (!scr_dashboard) return;

    char buf[128];

    // Update Top Status
    snprintf(buf, sizeof(buf), "\xF0\x9F\x93\xB6 %s | \xF0\x9F\x94\x8B %d%% (%.1fV) | \xF0\x9F\x94\xB5 %s", 
             wifi_ssid, battery_pct, battery_v, is_bluetooth_on ? "ON" : "OFF");
    if (lbl_top_status) lv_label_set_text(lbl_top_status, buf);

    // Update Center Temp/Hum
    snprintf(buf, sizeof(buf), "%.1f C  %.0f%%", temp, humidity);
    if (lbl_temp_hum) lv_label_set_text(lbl_temp_hum, buf);

    // Update Network Panel
    if (lbl_network) lv_label_set_text(lbl_network, ip_addr);

    // Update Uptime Panel
    if (lbl_uptime) lv_label_set_text(lbl_uptime, uptime_str);
}


