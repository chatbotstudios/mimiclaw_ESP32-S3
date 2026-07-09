#include "ui_manager.h"
#include "font_manager.h"
#include "lvgl.h"
#include "esp_random.h"

void ui_welcome_create(void) {
    scr_welcome = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_welcome, lv_color_hex(0x000000), 0);

    lv_obj_t *msg = lv_label_create(scr_welcome);
    lv_label_set_text(msg, "SCREEN 1");
    lv_obj_set_style_text_font(msg, font_manager_get_emoji_font(48), 0);
    lv_obj_set_style_text_color(msg, lv_color_hex(0x4285F4), 0);
    lv_obj_align(msg, LV_ALIGN_CENTER, 0, 0);
}
