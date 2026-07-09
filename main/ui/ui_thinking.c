#include "ui_manager.h"
#include "lvgl.h"

#ifdef CONFIG_BOARD_AMOLED_175
#include "bsp/esp-bsp.h"
#endif

static lv_obj_t *thinking_label = NULL;

void ui_thinking_create(void) {
    scr_thinking = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_thinking, lv_color_hex(0x000000), 0);

    thinking_label = lv_label_create(scr_thinking);
    lv_label_set_text(thinking_label, "SCREEN 2");
    lv_obj_set_style_text_color(thinking_label, lv_color_hex(0x4285F4), 0);
    lv_obj_set_style_transform_zoom(thinking_label, 768, 0); // 3x scale
    lv_obj_align(thinking_label, LV_ALIGN_CENTER, 0, 0);
}

void ui_thinking_update(const char *text) {
    if (!thinking_label) return;
    
#ifdef CONFIG_BOARD_AMOLED_175
    bsp_display_lock(0);
#endif

    lv_label_set_text(thinking_label, text);

#ifdef CONFIG_BOARD_AMOLED_175
    bsp_display_unlock();
#endif
}
