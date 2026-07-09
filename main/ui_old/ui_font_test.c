#include "ui_manager.h"
#include "font_manager.h"
#include "lvgl.h"

void ui_font_test_create(void) {
    scr_font_test = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_font_test, lv_color_hex(0x000000), 0);

    // Header
    lv_obj_t *header = lv_label_create(scr_font_test);
    lv_label_set_text(header, "SCREEN 4: Fonts");
    lv_obj_set_style_text_font(header, font_manager_get_core_font_24(), 0);
    lv_obj_set_style_text_color(header, lv_color_hex(0x4285F4), 0);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 20);

    // Core Font 24
    lv_obj_t *lbl_core24 = lv_label_create(scr_font_test);
    lv_label_set_text(lbl_core24, "Core Font 24px");
    lv_obj_set_style_text_font(lbl_core24, font_manager_get_core_font_24(), 0);
    lv_obj_set_style_text_color(lbl_core24, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(lbl_core24, LV_ALIGN_TOP_MID, 0, 80);

    // Core Font 48
    lv_obj_t *lbl_core48 = lv_label_create(scr_font_test);
    lv_label_set_text(lbl_core48, "Core Font 48px");
    lv_obj_set_style_text_font(lbl_core48, font_manager_get_core_font_48(), 0);
    lv_obj_set_style_text_color(lbl_core48, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(lbl_core48, LV_ALIGN_TOP_MID, 0, 140);

    // Dynamic TTF Font 24 (or fallback if SD missing)
    lv_obj_t *lbl_ttf24 = lv_label_create(scr_font_test);
    lv_label_set_text(lbl_ttf24, "TTF Emoji 24px \xF0\x9F\x9A\x80"); // Rocket emoji
    lv_obj_set_style_text_font(lbl_ttf24, font_manager_get_emoji_font(24), 0);
    lv_obj_set_style_text_color(lbl_ttf24, lv_color_hex(0x00FF00), 0);
    lv_obj_align(lbl_ttf24, LV_ALIGN_TOP_MID, 0, 240);

    // Dynamic TTF Font 48 (or fallback if SD missing)
    lv_obj_t *lbl_ttf48 = lv_label_create(scr_font_test);
    lv_label_set_text(lbl_ttf48, "TTF Emoji 48px \xF0\x9F\x94\xA5"); // Fire emoji
    lv_obj_set_style_text_font(lbl_ttf48, font_manager_get_emoji_font(48), 0);
    lv_obj_set_style_text_color(lbl_ttf48, lv_color_hex(0x00FF00), 0);
    lv_obj_align(lbl_ttf48, LV_ALIGN_TOP_MID, 0, 320);
}
