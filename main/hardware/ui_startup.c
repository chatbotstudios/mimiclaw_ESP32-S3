#include "display.h"
#include "lvgl.h"
#ifdef CONFIG_BOARD_AMOLED_175
#include "bsp/esp-bsp.h"
#else
#include "esp_lvgl_port.h"
#endif

static void anim_opa_cb(void * var, int32_t v) {
    lv_obj_set_style_text_opa((lv_obj_t *)var, v, 0);
}

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

void mimi_display_show_dashboard(const char *wifi_ssid, const char *ip_addr,
                                 float battery_v, int battery_pct, float temp,
                                 float humidity, bool is_recording,
                                 int unread_msgs, const char *uptime_str,
                                 bool is_bluetooth_on) {
#ifdef CONFIG_BOARD_AMOLED_175
    // Lock LVGL
    bsp_display_lock(0);

    lv_obj_t * scr = lv_scr_act();
    lv_obj_clean(scr); // Clear previous UI
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), 0);

    // Create a Flex Container for layout
    lv_obj_t * cont = lv_obj_create(scr);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(cont, 0, 0); // Transparent
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cont, 10, 0);

    // --- LOGO (Two crossed lines mimicking the Sparkle) ---
    static lv_point_t line_points1[] = { {0, 20}, {40, 20} };
    static lv_point_t line_points2[] = { {20, 0}, {20, 40} };

    lv_obj_t * logo_cont = lv_obj_create(cont);
    lv_obj_set_size(logo_cont, 40, 40);
    lv_obj_set_style_bg_opa(logo_cont, 0, 0);
    lv_obj_set_style_border_width(logo_cont, 0, 0);
    lv_obj_clear_flag(logo_cont, LV_OBJ_FLAG_SCROLLABLE);

    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 8);
    lv_style_set_line_color(&style_line, lv_color_hex(0x4285F4)); // Gemini Blue
    lv_style_set_line_rounded(&style_line, true);

    lv_obj_t * line1 = lv_line_create(logo_cont);
    lv_line_set_points(line1, line_points1, 2);
    lv_obj_add_style(line1, &style_line, 0);

    lv_obj_t * line2 = lv_line_create(logo_cont);
    lv_line_set_points(line2, line_points2, 2);
    lv_obj_add_style(line2, &style_line, 0);

    // --- TITLE ---
    lv_obj_t * title = lv_label_create(cont);
    lv_label_set_text(title, "WireBot");
    lv_obj_set_style_text_color(title, lv_color_hex(0x4285F4), 0);
    // Scale up text slightly
    lv_obj_set_style_transform_zoom(title, 384, 0); // 1.5x scale

    // --- WIFI STATUS ---
    bool wifi_on = (ip_addr != NULL && strlen(ip_addr) > 0 && strcmp(ip_addr, "0.0.0.0") != 0);
    
    lv_obj_t * wifi_label = lv_label_create(cont);
    lv_label_set_text(wifi_label, wifi_on ? "WiFi: ON" : "WiFi: OFF");
    lv_obj_set_style_text_color(wifi_label, wifi_on ? lv_color_hex(0x00FF00) : lv_color_hex(0xFF0000), 0);
    
    lv_obj_t * ip_label = lv_label_create(cont);
    lv_label_set_text(ip_label, wifi_on ? ip_addr : "No IP");
    lv_obj_set_style_text_color(ip_label, wifi_on ? lv_color_hex(0xFFFFFF) : lv_color_hex(0x646464), 0);

    // --- TEMPERATURE ---
    lv_obj_t * temp_label = lv_label_create(cont);
    lv_label_set_text_fmt(temp_label, "Temp: %.1fC", temp);
    
    uint32_t temp_color = 0xFF0000; // Default Red
    if (temp < 30.0) {
        temp_color = 0x00FF00; // Green
    } else if (temp <= 33.0) {
        temp_color = 0xFFA500; // Orange
    }
    lv_obj_set_style_text_color(temp_label, lv_color_hex(temp_color), 0);

    bsp_display_unlock();
#endif
}
