#include "ui_manager.h"
#include "lvgl.h"
#include "esp_random.h"

// Hardcoded messages to save Flash & avoid SD dependency for now.
static const char* welcome_messages[] = {
    "Hello, human. Ready to explore the universe?",
    "Mimi online. What shall we create today?",
    "Awaiting your command.",
    "Systems optimal. Ready for input.",
    "Good day! How can I assist you?",
    "Gemini connection established.",
    "Let's build something amazing.",
    "I am awake.",
    "Processing environment... ready.",
    "All sensors nominal."
};

void ui_welcome_create(void) {
    scr_welcome = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_welcome, lv_color_hex(0x000000), 0);

    int num_messages = sizeof(welcome_messages) / sizeof(welcome_messages[0]);
    int idx = esp_random() % num_messages;

    lv_obj_t *msg = lv_label_create(scr_welcome);
    lv_label_set_text(msg, welcome_messages[idx]);
    lv_obj_set_style_text_color(msg, lv_color_hex(0x4285F4), 0);
    lv_label_set_long_mode(msg, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(msg, LV_PCT(80));
    lv_obj_set_style_text_align(msg, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_transform_zoom(msg, 384, 0); // 1.5x scale
    lv_obj_align(msg, LV_ALIGN_CENTER, 0, 0);
    
    // Note: Touch and gesture events are added in ui_manager.c globally.
}
