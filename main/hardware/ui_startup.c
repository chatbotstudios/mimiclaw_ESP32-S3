#include "display.h"
#include "lvgl.h"
#ifdef CONFIG_BOARD_AMOLED_175
#include "bsp/esp-bsp.h"
#else
#include "esp_lvgl_port.h"
#endif



void mimi_display_show_startup_animation(void) {
#ifdef CONFIG_BOARD_AMOLED_175
    // Lock LVGL
    bsp_display_lock(0);

    // Create a screen
    lv_obj_t * scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), 0);

    // Create MIMI label
    lv_obj_t * label = lv_label_create(scr);
    lv_label_set_text(label, "MIMI");
    lv_obj_set_style_text_color(label, lv_color_hex(0x4285F4), 0); // Gemini Blue
    
    // Zoom it 3x to make it large
    lv_obj_set_style_transform_zoom(label, 768, 0); // 256 is 1x, 768 is 3x
    lv_obj_center(label);

    bsp_display_unlock();
#endif
}

static lv_obj_t *lbl_temp = NULL;
static lv_obj_t *lbl_hum = NULL;
static lv_obj_t *arc_batt = NULL;
static lv_obj_t *lbl_batt = NULL;
static lv_obj_t *lbl_status = NULL;
static lv_obj_t *lbl_wifi = NULL;
static lv_obj_t *lbl_ip = NULL;
static bool dashboard_created = false;

void mimi_display_show_dashboard(const char *wifi_ssid, const char *ip_addr,
                                 float battery_v, int battery_pct, float temp,
                                 float humidity, bool is_recording,
                                 int unread_msgs, const char *uptime_str,
                                 bool is_bluetooth_on) {
#ifdef CONFIG_BOARD_AMOLED_175
    bsp_display_lock(0);
    lv_obj_t * scr = lv_scr_act();

    if (!dashboard_created) {
        lv_obj_clean(scr);
        lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), 0);

        // Header Title
        lv_obj_t *title = lv_label_create(scr);
        lv_label_set_text(title, "MimiClaw");
        // Using default font, scaled up slightly
        lv_obj_set_style_text_color(title, lv_color_hex(0x4285F4), 0); // Gemini Blue
        lv_obj_set_style_transform_zoom(title, 384, 0); // 1.5x
        lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 40);

        // Wifi Label
        lbl_wifi = lv_label_create(scr);
        lv_obj_set_style_text_color(lbl_wifi, lv_color_hex(0x4285F4), 0);
        lv_obj_align(lbl_wifi, LV_ALIGN_TOP_MID, 0, 80);

        lbl_ip = lv_label_create(scr);
        lv_obj_set_style_text_color(lbl_ip, lv_color_hex(0xFFFFFF), 0); // White
        lv_obj_align(lbl_ip, LV_ALIGN_TOP_MID, 0, 100);

        // Large Temperature
        lbl_temp = lv_label_create(scr);
        lv_obj_set_style_text_color(lbl_temp, lv_color_hex(0xFFFFFF), 0); // White
        lv_obj_set_style_transform_zoom(lbl_temp, 768, 0); // 3x scale
        lv_obj_align(lbl_temp, LV_ALIGN_CENTER, 0, -20);

        // Humidity
        lbl_hum = lv_label_create(scr);
        lv_obj_set_style_text_color(lbl_hum, lv_color_hex(0x4285F4), 0); // Blue
        lv_obj_set_style_transform_zoom(lbl_hum, 384, 0); // 1.5x scale
        lv_obj_align(lbl_hum, LV_ALIGN_CENTER, 0, 40);

        // Battery Arc
        arc_batt = lv_arc_create(scr);
        lv_obj_set_size(arc_batt, 120, 120);
        lv_arc_set_bg_angles(arc_batt, 140, 400);
        lv_arc_set_angles(arc_batt, 140, 220);
        lv_obj_set_style_arc_width(arc_batt, 10, LV_PART_MAIN);
        lv_obj_set_style_arc_width(arc_batt, 10, LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(arc_batt, lv_color_hex(0x4285F4), LV_PART_INDICATOR); // Blue arc
        lv_obj_remove_style(arc_batt, NULL, LV_PART_KNOB); // Hide knob
        lv_obj_clear_flag(arc_batt, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_align(arc_batt, LV_ALIGN_BOTTOM_MID, 0, -30);

        lbl_batt = lv_label_create(scr);
        lv_obj_set_style_text_color(lbl_batt, lv_color_hex(0xFFFFFF), 0);
        lv_obj_align_to(lbl_batt, arc_batt, LV_ALIGN_CENTER, 0, 0);

        // Status Line
        lbl_status = lv_label_create(scr);
        lv_obj_set_style_text_color(lbl_status, lv_color_hex(0xAAAAAA), 0); // Light gray
        lv_obj_align(lbl_status, LV_ALIGN_BOTTOM_MID, 0, -10);

        dashboard_created = true;
    }

    // --- UPDATE VALUES ---
    bool wifi_on = (ip_addr != NULL && strlen(ip_addr) > 0 && strcmp(ip_addr, "0.0.0.0") != 0);
    lv_label_set_text(lbl_wifi, wifi_on ? "WiFi: ON" : "WiFi: OFF");
    lv_label_set_text(lbl_ip, wifi_on ? ip_addr : "No IP");

    lv_label_set_text_fmt(lbl_temp, "%.1f C", temp);
    lv_label_set_text_fmt(lbl_hum, "Hum: %.0f%%", humidity);

    lv_arc_set_value(arc_batt, battery_pct);
    lv_label_set_text_fmt(lbl_batt, "%d%%", battery_pct);

    if (is_recording) {
        lv_label_set_text(lbl_status, "Recording Audio...");
        lv_obj_set_style_text_color(lbl_status, lv_color_hex(0xFF0000), 0); // Red for recording
    } else {
        lv_label_set_text_fmt(lbl_status, "Up: %s", uptime_str ? uptime_str : "--");
        lv_obj_set_style_text_color(lbl_status, lv_color_hex(0xAAAAAA), 0);
    }

    bsp_display_unlock();
#endif
}
