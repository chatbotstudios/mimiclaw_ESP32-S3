#include "ui_manager.h"
#include "font_manager.h"
#include "lvgl.h"

#ifdef CONFIG_BOARD_AMOLED_175
#include "bsp/esp-bsp.h"
#endif

static void splash_timer_cb(lv_timer_t *t) {
    ui_manager_switch_page(PAGE_WELCOME);
    lv_timer_del(t);
}

void ui_splash_show(void) {
#ifdef CONFIG_BOARD_AMOLED_175
    bsp_display_lock(0);
#endif

    lv_obj_t *splash = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(splash, lv_color_hex(0x000000), 0);

    lv_obj_t *lbl_mimi = lv_label_create(splash);
    lv_label_set_text(lbl_mimi, "MIMI");
    lv_obj_set_style_text_font(lbl_mimi, font_manager_get_core_font_48(), 0);
    lv_obj_set_style_text_color(lbl_mimi, lv_color_hex(0x4285F4), 0); // Gemini Blue
    lv_obj_align(lbl_mimi, LV_ALIGN_CENTER, 0, -20);

    lv_obj_t *subtitle = lv_label_create(splash);
    lv_label_set_text(subtitle, "Claw \xEF\x80\xA2 Gemini"); // Bullet point
    lv_obj_set_style_text_color(subtitle, lv_color_hex(0x888888), 0);
    lv_obj_set_style_transform_zoom(subtitle, 384, 0); // 1.5x scale
    lv_obj_align(subtitle, LV_ALIGN_CENTER, 0, 40);

    lv_scr_load(splash);

    // Auto transition after 2500ms
    lv_timer_create(splash_timer_cb, 2500, NULL);

#ifdef CONFIG_BOARD_AMOLED_175
    bsp_display_unlock();
#endif
}
